#pragma once
#include "superfetch_h.h"
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <hashtable.h>
#include <module_context.h>
#include <syslib.hpp>

extern PMODULE_CONTEXT pCtx;

namespace spf2 {
    struct memory_range {
        std::uint64_t pfn = 0;
        std::size_t   page_count = 0;
    };

    struct translation {
        void const* virt_page = nullptr;
        std::uint64_t phys_page = 0;
    };

    struct memory_map {
        std::vector<memory_range> ranges;
        std::vector<translation>  translations;
    };
    static NTSTATUS spf_raise_privilege() {
        NTSTATUS status = ERROR_SUCCESS;
        HANDLE hToken, hProcess;
        TOKEN_PRIVILEGES tp;
        LUID luid, luid2;
        LookupPrivilegeValueW(L"", L"SeDebugPrivilege", &luid);
        LookupPrivilegeValueW(L"", L"SeProfileSingleProcessPrivilege", &luid2);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        status = pCtx->psyslib->nt_call_3arg(HASH_NtOpenProcessToken, ((HANDLE)(LONG_PTR)-1), (PVOID)(TOKEN_READ | TOKEN_WRITE), &hToken);
        if (!NT_SUCCESS(status)) return status;
        
        status = pCtx->psyslib->nt_call(HASH_NtAdjustPrivilegesToken, (HANDLE)hToken, FALSE, &tp, sizeof(tp), 0, 0);
        if (!NT_SUCCESS(status)) return status;
        
        tp.Privileges[0].Luid = luid2;
        status = pCtx->psyslib->nt_call(HASH_NtAdjustPrivilegesToken, (HANDLE)hToken, FALSE, &tp, sizeof(tp), 0, 0);
        if (!NT_SUCCESS(status)) return status;

        pCtx->psyslib->nt_call_1arg(HASH_NtClose, hToken);
        pCtx->psyslib->nt_call_1arg(HASH_NtClose, hProcess);

        return status;
    }

    static NTSTATUS spf_query_superfetch_info(
        SUPERFETCH_INFORMATION_CLASS info_class,
        void* buffer,
        ULONG                        length,
        ULONG* return_length = nullptr
    ) {
        SUPERFETCH_INFORMATION sfi = {};
        sfi.InfoClass = info_class;
        sfi.Data = buffer;
        sfi.Length = length;

        return pCtx->psyslib->nt_call(
            HASH_NtQuerySystemInformation,
            SystemSuperfetchInformation,
            &sfi,
            sizeof(sfi),
            return_length
        );
    }

    static std::vector<memory_range> spf_query_memory_ranges_v1() {
        ULONG buffer_length = 0;

        PF_MEMORY_RANGE_INFO_V1 probe = {};
        if ((NTSTATUS)0xC0000023 != spf_query_superfetch_info(
            SuperfetchMemoryRangesQuery, &probe, sizeof(probe), &buffer_length)) {
            return {};
        }

        std::vector<std::uint8_t> buffer(buffer_length);
        auto* info = reinterpret_cast<PF_MEMORY_RANGE_INFO_V1*>(buffer.data());
        info->Version = 1;

        if (!NT_SUCCESS(spf_query_superfetch_info(
            SuperfetchMemoryRangesQuery, info, buffer_length))) {
            return {};
        }

        std::vector<memory_range> ranges;
        ranges.reserve(info->RangeCount);

        for (std::uint32_t i = 0; i < info->RangeCount; ++i) {
            memory_range r;
            r.pfn = info->Ranges[i].BasePfn;
            r.page_count = static_cast<std::size_t>(info->Ranges[i].PageCount);
            ranges.push_back(r);
        }

        return ranges;
    }

    static std::vector<memory_range> spf_query_memory_ranges_v2() {
        ULONG buffer_length = 0;
        PF_MEMORY_RANGE_INFO_V2 probe = {};
        if ((NTSTATUS)0xC0000023 != spf_query_superfetch_info(
            SuperfetchMemoryRangesQuery, &probe, sizeof(probe), &buffer_length)) {
            return {};
        }

        std::vector<std::uint8_t> buffer(buffer_length);
        auto* info = reinterpret_cast<PF_MEMORY_RANGE_INFO_V2*>(buffer.data());
        info->Version = 2;

        if (!NT_SUCCESS(spf_query_superfetch_info(
            SuperfetchMemoryRangesQuery, info, buffer_length))) {
            return {};
        }

        std::vector<memory_range> ranges;
        ranges.reserve(info->RangeCount);

        for (std::uint32_t i = 0; i < info->RangeCount; ++i) {
            memory_range r;
            r.pfn = info->Ranges[i].BasePfn;
            r.page_count = static_cast<std::size_t>(info->Ranges[i].PageCount);
            ranges.push_back(r);
        }

        return ranges;
    }

    static std::vector<memory_range> spf_query_memory_ranges() {
        auto r = spf_query_memory_ranges_v1();
        if (!r.empty())
            return r;
        return spf_query_memory_ranges_v2();
    }
    static std::uint64_t spf_find_phys_page(
        std::vector<translation> const& translations,
        void const* virt_page
    ) {
        for (auto const& t : translations) {
            if (t.virt_page == virt_page)
                return t.phys_page;
        }
        return 0;
    }

    static bool spf_mm_current(memory_map* out_mm) {
        if (!out_mm) return false;
        out_mm->ranges.clear();
        out_mm->translations.clear();

        if (!NT_SUCCESS(spf_raise_privilege())) {
            SetLastError(ERROR_ACCESS_DENIED);
            return false;
        }

        out_mm->ranges = spf_query_memory_ranges();
        if (out_mm->ranges.empty()) {
            return false;
        }

        for (auto const& range : out_mm->ranges) {
            std::uint64_t base_pfn = range.pfn;
            std::size_t   page_count = range.page_count;
            std::size_t buffer_length =
                sizeof(PF_PFN_PRIO_REQUEST) + sizeof(MMPFN_IDENTITY) * page_count;

            std::vector<std::uint8_t> buffer(buffer_length);
            auto* req = reinterpret_cast<PF_PFN_PRIO_REQUEST*>(buffer.data());

            req->Version = 1;
            req->RequestFlags = 1;
            req->PfnCount = static_cast<ULONG>(page_count);

            for (std::uint64_t i = 0; i < page_count; ++i) {
                req->PageData[i].PageFrameIndex = base_pfn + i;
            }

            if (!NT_SUCCESS(spf_query_superfetch_info(
                SuperfetchPfnQuery,
                req,
                static_cast<ULONG>(buffer_length)))) {
                return false;
            }

            for (std::uint64_t i = 0; i < page_count; ++i) {
                void const* virt = req->PageData[i].u2.VirtualAddress;
                if (!virt) continue;

                translation t;
                t.virt_page = virt;
                t.phys_page = (base_pfn + i) << 12;
                out_mm->translations.push_back(t);
            }
        }

        return true;
    }

    static std::uint64_t spf_mm_translate(memory_map const* mm, void const* address) {
        if (!mm || !address) return 0;

        std::uint64_t a = reinterpret_cast<std::uint64_t>(address);
        void const* aligned = reinterpret_cast<void const*>(a & ~0xFFFull);

        std::uint64_t phys_page = spf_find_phys_page(mm->translations, aligned);
        if (!phys_page) return 0;

        return phys_page + (a & 0xFFF);
    }

    static void spf_mm_free(memory_map const* mm) {
        mm->ranges.~vector();
        mm->translations.~vector();
    }
}