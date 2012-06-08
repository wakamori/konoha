K.import("konoha");
K.import("konoha.curl");
K.import("konoha.json");

void test() {
	Curl c = new Curl();
	String str = "";
	c.setOpt(CURLOPT_URL, "http://localhost:7474/db/data/node/60");
	c.setOpt(CURLOPT_WRITEDATA, str);
	c.perform();
	p(str);

	Json json = new Json();
	Json parsed = json.parse(str);
}

test();
