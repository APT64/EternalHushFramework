package eternalhush.dlcore;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class ArgumentPacker {
    private ByteBuffer arg_buffer;
    private ArrayList<Argument> temp_buffer;
    private long final_arglen = 0;

    public ArgumentPacker() {
        temp_buffer = new ArrayList<>();
    }

    public void addArgI(String Name, long i) {
        int buf_len = 4 + Name.length() + 4 + 8 + 1;
        byte nullbyte = 0x0;
        ByteBuffer b = ByteBuffer.allocate(buf_len);
        b.putInt(Name.length() + 1);
        b.put(Name.getBytes());
        b.put(nullbyte);
        b.put("$INT".getBytes());
        b.putLong(i);
        final_arglen += buf_len;
        Argument a = new Argument();
        a.arg_ptr = b.array();
        a.arg_size = buf_len;
        temp_buffer.add(a);
    }

    public void addArgS(String Name, String s) {
        int buf_len = 4 + Name.length() + 4 + 4 + s.length() + 2;
        byte nullbyte = 0x0;
        ByteBuffer b = ByteBuffer.allocate(buf_len);
        b.putInt(Name.length() + 1);
        b.put(Name.getBytes());
        b.put(nullbyte);
        b.put("$STR".getBytes());
        b.putInt(s.length() + 1);
        b.put(s.getBytes());
        b.put(nullbyte);
        final_arglen += buf_len;
        Argument a = new Argument();
        a.arg_ptr = b.array();
        a.arg_size = buf_len;
        temp_buffer.add(a);
    }

    public void addArgF(String Name) {
        addArgI(Name, 1);
    }

    public ByteBuffer pack() {
        arg_buffer = ByteBuffer.allocate((int) final_arglen);
        for (Argument a : temp_buffer) {
            arg_buffer.put(a.arg_ptr);
        }
        return arg_buffer;
    }

    public long getSize() {
        return final_arglen;
    }

    class Argument {
        public byte[] arg_ptr;
        public int arg_size;
    }
}