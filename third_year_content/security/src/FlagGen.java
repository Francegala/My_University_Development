import java.io.PrintStream;
import java.util.Base64;
public class FlagGen {
    public FlagGen() {
    	
    }
    public static void main(String[] var0) {
        String var1 = "Not an actual flag";
        System.out.println("flag{" + xorHex(generateFirstPart(),"cc160c") + xorHex(generateLastPart(),"96c540") + "}");
    }

    public static String generateFirstPart() {
        return "5df395";
    }

    public static String generateLastPart() {
        return "9e25c7";
    }

    public static String xorHex(String var0, String var1) {
        char[] var2 = new char[var0.length()];

        for(int var3 = 0; var3 < var2.length; ++var3) {
            var2[var3] = toHex(fromHex(var0.charAt(var3)) ^ fromHex(var1.charAt(var3)));
        }

        return new String(var2);
    }

    public static void printHint() {
        byte[] var0 = Base64.getDecoder().decode("VGhlIGZpZWxkIGZsYWcgb2YgdGhlIGZ1bmN0aW9uIG1haW4gc2hvdWxkIGFjdHVhbGx5IGJ5IGRlZmluZWQgYnkgY29uY2F0ZW5pbmcgdGhlIHhvciBvZiB0aGUgb3V0cHV0IG9mIGdlbmVyYXRlRmlyc3RQYXJ0IHdpdGggY2MxNjBjIHdpdGggdGhlIHhvciBvZiB0aGUgb3V0cHV0IG9mIGdlbmVyYXRlTGFzdFBhcnQgd2l0aCA5NmM1NDAuIFRoZSByZXN1bHRpbmcgc3RyaW5nIHNob3VsZCBiZSBpbiBsb3dlcmNhc2UgYW5kIGVuY2Fwc3VsYXRlZCBpbiBmbGFnXHsuLi5cfS4K");
        PrintStream var10000 = System.out;
        String var10001 = new String(var0);
        var10000.println("Hint: " + var10001);
    }

    private static int fromHex(char var0) {
        if (var0 >= '0' && var0 <= '9') {
            return var0 - 48;
        } else if (var0 >= 'A' && var0 <= 'F') {
            return var0 - 65 + 10;
        } else if (var0 >= 'a' && var0 <= 'f') {
            return var0 - 97 + 10;
        } else {
            throw new IllegalArgumentException();
        }
    }

    private static char toHex(int var0) {
        if (var0 >= 0 && var0 <= 15) {
            return "0123456789ABCDEF".charAt(var0);
        } else {
            throw new IllegalArgumentException();
        }
    }
}

