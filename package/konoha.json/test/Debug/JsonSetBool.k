K.import("konoha");
//K.import("konoha.float");
K.import("konoha.string");
K.import("konoha.json");
K.import("konoha.bytes");
K.import("posix.file");

void main() {
	FILE f = System.fopen("../../../package/konoha.json/test/Debug/json_sample.txt", "r");
	int bufsize = 256;
	Bytes buf = Bytes.new(bufsize);
	buf.setAll(0); // zero clear
	int max = f.read(buf, 0, bufsize);
	String str = buf.toString().substring(0, max);
	Json j = new Json();
	Json parsed = j.parse(str);
	parsed.setBool("badd", true);
	p(parsed.getBool("badd"));
}

main();
