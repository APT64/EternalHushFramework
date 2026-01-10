from additional.clingyspider.api import *

class data_types:
    UCHAR, CHAR, BYTE, BOOLEAN = 1, 1, 1, 1
    ULONG, LONG, DWORD, INT, PVOID32, HANDLE32, PTR32, SOCKET32, HANDLE32, HINTERNET32 = 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
    PVOID64, PTR64, LONGLONG, HANDLE64, HINTERNET64, SOCKET64, DWORD64 = 8,8,8,8,8,8,8
    WORD, SHORT, USHORT = 2, 2, 2

class PEB64_OFFSETLIST:
    OFFSET_PEB_SEGMENT = 0x60
    InheritedAddressSpace = 0x0
    ReadImageFileExecOptions = 0x1
    BeingDebugged = 0x2
    BitField = 0x3
    ImageUsesLargePages = 0x3
    IsProtectedProcess = 0x3
    IsLegacyProcess = 0x3
    IsImageDynamicallyRelocated = 0x3
    SkipPatchingUser32Forwarders = 0x3
    IsPackagedProcess = 0x3
    IsAppContainer = 0x3
    SpareBits = 0x3
    Mutant = 0x8
    ImageBaseAddress = 0x10
    Ldr = 0x18
    ProcessParameters = 0x20
    SubSystemData = 0x28
    ProcessHeap = 0x30
    FastPebLock = 0x38
    AtlThunkSListPtr = 0x40
    IFEOKey = 0x48
    CrossProcessFlags = 0x50
    ProcessInJob = 0x50
    ProcessInitializing = 0x50
    ProcessUsingVEH = 0x50
    ProcessUsingVCH = 0x50
    ProcessUsingFTH = 0x50
    ReservedBits0 = 0x50
    KernelCallbackTable = 0x58
    UserSharedInfoPtr = 0x58
    SystemReserved = 0x60
    AtlThunkSListPtr32 = 0x64
    ApiSetMap = 0x68
    TlsExpansionCounter = 0x70
    TlsBitmap = 0x78
    TlsBitmapBits = 0x80
    ReadOnlySharedMemoryBase = 0x88
    HotpatchInformation = 0x90
    ReadOnlyStaticServerData = 0x98
    AnsiCodePageData = 0xa0
    OemCodePageData = 0xa8
    UnicodeCaseTableData = 0xb0
    NumberOfProcessors = 0xb8
    NtGlobalFlag = 0xbc
    CriticalSectionTimeout = 0xc0
    HeapSegmentReserve = 0xc8
    HeapSegmentCommit = 0xd0
    HeapDeCommitTotalFreeThreshold = 0xd8
    HeapDeCommitFreeBlockThreshold = 0xe0
    NumberOfHeaps = 0xe8
    MaximumNumberOfHeaps = 0xec
    ProcessHeaps = 0xf0
    GdiSharedHandleTable = 0xf8
    ProcessStarterHelper = 0x100
    GdiDCAttributeList = 0x108
    LoaderLock = 0x110
    OSMajorVersion = 0x118
    OSMinorVersion = 0x11c
    OSBuildNumber = 0x120
    OSCSDVersion = 0x122
    OSPlatformId = 0x124
    ImageSubsystem = 0x128
    ImageSubsystemMajorVersion = 0x12c
    ImageSubsystemMinorVersion = 0x130
    ActiveProcessAffinityMask = 0x138
    GdiHandleBuffer = 0x140
    PostProcessInitRoutine = 0x230
    TlsExpansionBitmap = 0x238
    TlsExpansionBitmapBits = 0x240
    SessionId = 0x2c0
    AppCompatFlags = 0x2c8
    AppCompatFlagsUser = 0x2d0
    pShimData = 0x2d8
    AppCompatInfo = 0x2e0
    CSDVersion = 0x2e8
    ActivationContextData = 0x2f8
    ProcessAssemblyStorageMap = 0x300
    SystemDefaultActivationContextData = 0x308
    SystemAssemblyStorageMap = 0x310
    MinimumStackCommit = 0x318
    FlsCallback = 0x320
    FlsListHead = 0x328
    FlsBitmap = 0x338
    FlsBitmapBits = 0x340
    FlsHighIndex = 0x350
    WerRegistrationData = 0x358
    WerShipAssertPtr = 0x360
    pUnused = 0x368
    pImageHeaderHash = 0x370
    TracingFlags = 0x378
    HeapTracingEnabled = 0x378
    CritSecTracingEnabled = 0x378
    LibLoaderTracingEnabled = 0x378
    SpareTracingBits = 0x378
    CsrServerReadOnlySharedMemoryBase = 0x380

    TOTAL_SIZE = 0x388

class PEB32_OFFSETLIST:
    OFFSET_PEB_SEGMENT = 0x30
    InheritedAddressSpace = 0x0
    ReadImageFileExecOptions = 0x1
    BeingDebugged = 0x2
    BitField = 0x3
    ImageUsesLargePages = 0x3
    IsProtectedProcess = 0x3
    IsLegacyProcess = 0x3
    IsImageDynamicallyRelocated = 0x3
    SkipPatchingUser32Forwarders = 0x3
    IsPackagedProcess = 0x3
    IsAppContainer = 0x3
    SpareBits = 0x3
    Mutant = 0x4
    ImageBaseAddress = 0x8
    Ldr = 0xc
    ProcessParameters = 0x10
    SubSystemData = 0x14
    ProcessHeap = 0x18
    FastPebLock = 0x1c
    AtlThunkSListPtr = 0x20
    IFEOKey = 0x24
    CrossProcessFlags = 0x28
    ProcessInJob = 0x28
    ProcessInitializing = 0x28
    ProcessUsingVEH = 0x28
    ProcessUsingVCH = 0x28
    ProcessUsingFTH = 0x28
    ReservedBits0 = 0x28
    KernelCallbackTable = 0x2c
    UserSharedInfoPtr = 0x2c
    SystemReserved = 0x30
    AtlThunkSListPtr32 = 0x34
    ApiSetMap = 0x38
    TlsExpansionCounter = 0x3c
    TlsBitmap = 0x40
    TlsBitmapBits = 0x44
    ReadOnlySharedMemoryBase = 0x4c
    HotpatchInformation = 0x50
    ReadOnlyStaticServerData = 0x54
    AnsiCodePageData = 0x58
    OemCodePageData = 0x5c
    UnicodeCaseTableData = 0x60
    NumberOfProcessors = 0x64
    NtGlobalFlag = 0x68
    CriticalSectionTimeout = 0x70
    HeapSegmentReserve = 0x78
    HeapSegmentCommit = 0x7c
    HeapDeCommitTotalFreeThreshold = 0x80
    HeapDeCommitFreeBlockThreshold = 0x84
    NumberOfHeaps = 0x88
    MaximumNumberOfHeaps = 0x8c
    ProcessHeaps = 0x90
    GdiSharedHandleTable = 0x94
    ProcessStarterHelper = 0x98
    GdiDCAttributeList = 0x9c
    LoaderLock = 0xa0
    OSMajorVersion = 0xa4
    OSMinorVersion = 0xa8
    OSBuildNumber = 0xac
    OSCSDVersion = 0xae
    OSPlatformId = 0xb0
    ImageSubsystem = 0xb4
    ImageSubsystemMajorVersion = 0xb8
    ImageSubsystemMinorVersion = 0xbc
    ActiveProcessAffinityMask = 0xc0
    GdiHandleBuffer = 0xc4
    PostProcessInitRoutine = 0x14c
    TlsExpansionBitmap = 0x150
    TlsExpansionBitmapBits = 0x154
    SessionId = 0x1d4
    AppCompatFlags = 0x1d8
    AppCompatFlagsUser = 0x1e0
    pShimData = 0x1e8
    AppCompatInfo = 0x1ec
    CSDVersion = 0x1f0
    ActivationContextData = 0x1f8
    ProcessAssemblyStorageMap = 0x1fc
    SystemDefaultActivationContextData = 0x200
    SystemAssemblyStorageMap = 0x204
    MinimumStackCommit = 0x208
    FlsCallback = 0x20c
    FlsListHead = 0x210
    FlsBitmap = 0x218
    FlsBitmapBits = 0x21c
    FlsHighIndex = 0x22c
    WerRegistrationData = 0x230
    WerShipAssertPtr = 0x234
    pUnused = 0x238
    pImageHeaderHash = 0x23c
    TracingFlags = 0x240
    HeapTracingEnabled = 0x240
    CritSecTracingEnabled = 0x240
    LibLoaderTracingEnabled = 0x240
    SpareTracingBits = 0x240
    CsrServerReadOnlySharedMemoryBase = 0x248

    TOTAL_SIZE = 0x250

class PEB_LDR_DATA64_OFFSETLIST():
    Length = 0x0
    Initialized = 0x4
    SsHandle = 0x8
    InLoadOrderModuleList = 0x10
    InMemoryOrderModuleList = 0x20
    InInitializationOrderModuleList = 0x30
    EntryInProgress = 0x40
    ShutdownInProgress = 0x48
    ShutdownThreadId = 0x50

    TOTAL_SIZE = 0x58

class PEB_LDR_DATA32_OFFSETLIST():
    Length = 0x0
    Initialized = 0x4
    SsHandle = 0x8
    InLoadOrderModuleList = 0xc
    InMemoryOrderModuleList = 0x14
    InInitializationOrderModuleList = 0x1c
    EntryInProgress = 0x24
    ShutdownInProgress = 0x28
    ShutdownThreadId = 0x2c

    TOTAL_SIZE = 0x30

class LIST_ENTRY64_OFFSETLIST():
    Flink = 0x0
    Blink = 0x8

    TOTAL_SIZE = 0x10

class LIST_ENTRY32_OFFSETLIST():
    Flink = 0x0
    Blink = 0x4

    TOTAL_SIZE = 0x8

class RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST():
    MaximumLength = 0x0
    Length = 0x4
    Flags = 0x8
    DebugFlags = 0xc
    ConsoleHandle = 0x10
    ConsoleFlags = 0x18
    StandardInput = 0x20
    StandardOutput = 0x28
    StandardError = 0x30
    CurrentDirectory = 0x38
    DllPath = 0x50
    ImagePathName = 0x60
    CommandLine = 0x70
    Environment = 0x80
    StartingX = 0x88
    StartingY = 0x8c
    CountX = 0x90
    CountY = 0x94
    CountCharsX = 0x98
    CountCharsY = 0x9c
    FillAttribute = 0xa0
    WindowFlags = 0xa4
    ShowWindowFlags = 0xa8
    WindowTitle = 0xb0
    DesktopInfo = 0xc0
    ShellInfo = 0xd0
    RuntimeData = 0xe0
    CurrentDirectores = 0xf0
    EnvironmentSize = 0x3f0
    EnvironmentVersion = 0x3f8
    PackageDependencyData = 0x400
    ProcessGroupId = 0x408

    TOTAL_SIZE = 0x410

class RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST():
    MaximumLength = 0x0
    Length = 0x4
    Flags = 0x8
    DebugFlags = 0xc
    ConsoleHandle = 0x10
    ConsoleFlags = 0x14
    StandardInput = 0x18
    StandardOutput = 0x1c
    StandardError = 0x20
    CurrentDirectory = 0x24
    DllPath = 0x30
    ImagePathName = 0x38
    CommandLine = 0x40
    Environment = 0x48
    StartingX = 0x4c
    StartingY = 0x50
    CountX = 0x54
    CountY = 0x58
    CountCharsX = 0x5c
    CountCharsY = 0x60
    FillAttribute = 0x64
    WindowFlags = 0x68
    ShowWindowFlags = 0x6c
    WindowTitle = 0x70
    DesktopInfo = 0x78
    ShellInfo = 0x80
    RuntimeData = 0x88
    CurrentDirectores = 0x90
    EnvironmentSize = 0x290
    EnvironmentVersion = 0x294
    PackageDependencyData = 0x298
    ProcessGroupId = 0x29c

    TOTAL_SIZE = 0x2a0

class CURDIR64_OFFSETLIST():
    DosPath = 0x0
    Handle = 0x10

    TOTAL_SIZE = 0x18

class CURDIR32_OFFSETLIST():
    DosPath = 0x0
    Handle = 0x8

    TOTAL_SIZE = 0xc

class UNICODE_STRING64_OFFSETLIST():
    Length = 0x0
    MaximumLength = 0x2
    Buffer = 0x8

    TOTAL_SIZE = 0x10

class UNICODE_STRING32_OFFSETLIST():
    Length = 0x0
    MaximumLength = 0x2
    Buffer = 0x4

    TOTAL_SIZE = 0x8
    
class STRING64_OFFSETLIST():
    Length = 0x0
    MaximumLength = 0x2
    Buffer = 0x8

    TOTAL_SIZE = 0x10

class STRING32_OFFSETLIST():
    Length = 0x0
    MaximumLength = 0x2
    Buffer = 0x4

    TOTAL_SIZE = 0x8

class RTL_DRIVE_LETTER_CURDIR64_OFFSETLIST():
    Flags = 0x0
    Length = 0x2
    TimeStamp = 0x4
    DosPath = 0x8

    TOTAL_SIZE = 0x18

class RTL_DRIVE_LETTER_CURDIR32_OFFSETLIST():
    Flags = 0x0
    Length = 0x2
    TimeStamp = 0x4
    DosPath = 0x8

    TOTAL_SIZE = 0x10

class LDR_DATA_TABLE_ENTRY32_OFFSETLIST():
    InLoadOrderLinks = 0x0
    InMemoryOrderLinks = 0x8
    InInitializationOrderLinks = 0x10
    DllBase = 0x18
    EntryPoint = 0x1c
    SizeOfImage = 0x20
    FullDllName = 0x24
    BaseDllName = 0x2c
    Flags= 0x34
    ObsoleteLoadCount = 0x38
    TlsIndex = 0x3a
    HashLinks = 0x3c
    TimeDateStamp = 0x44
    EntryPointActivationContext = 0x48
    PatchInformation = 0x4c
    DdagNode = 0x50
    NodeModuleLink = 0x54
    SnapContext = 0x5c
    ParentDllBase = 0x60
    SwitchBackContext = 0x64
    BaseAddressIndexNode = 0x68
    MappingInfoIndexNode = 0x74
    OriginalBase = 0x80
    LoadTime = 0x88
    BaseNameHashValue = 0x90
    LoadReason = 0x94

    TOTAL_SIZE = 0x98

class LDR_DATA_TABLE_ENTRY64_OFFSETLIST():
    InLoadOrderLinks         = 0x0
    InMemoryOrderLinks       = 0x10
    InInitializationOrderLinks = 0x20
    DllBase                  = 0x30
    EntryPoint               = 0x38
    SizeOfImage              = 0x40
    FullDllName              = 0x48
    BaseDllName              = 0x58
    Flags = 0x68
    ObsoleteLoadCount        = 0x6c
    TlsIndex                 = 0x6e
    HashLinks                = 0x70
    TimeDateStamp            = 0x80
    EntryPointActivationContext = 0x88
    PatchInformation         = 0x90
    DdagNode                 = 0x98
    NodeModuleLink           = 0xa0
    SnapContext              = 0xb0
    ParentDllBase            = 0xb8
    SwitchBackContext        = 0xc0
    BaseAddressIndexNode     = 0xc8
    MappingInfoIndexNode     = 0xe0
    OriginalBase             = 0xf8
    LoadTime                 = 0x100
    BaseNameHashValue        = 0x108
    LoadReason               = 0x10c

    TOTAL_SIZE = 0x110