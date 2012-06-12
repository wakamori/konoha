K.import("konoha");
K.import("konoha.curl");
K.import("konoha.json");

void test() {
	Curl c = new Curl();
	String str = "";
	c.setOpt(CURLOPT_URL, "http://localhost:7474/db/data/node/300");
	c.setOpt(CURLOPT_WRITEDATA, str);
	c.perform();
	p(str);

	Json json = new Json();
	Json parsed = json.parse(str);
	Json data = parsed.get("data");
	p(data.getInt("value"));
	p(data.getString("title"));
	//Json data = self.get("data");
	//p(data);
	//p(data.get("value"));
	//p(parsed.get("data"));
}

test();
