#include <stdio.h>
#include <curl/curl.h>

int main(void)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) { return -1; }

    curl_easy_setopt(curl, CURLOPT_URL, "http://google.com");
}
