#include <stdio.h>
#include <curl/curl.h>
#include <alert.h>

void send_alert(struct sockaddr_in *server_info){
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/alerts/receive");//POST URL set
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "Warning!Elderly fell!");//POST MESSAGE set
		res = curl_easy_perform(curl);//execute POST
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() FAILED: %s\n", curl_easy_strerror(res));//check if POST SUCCESS
		curl_easy_cleanup(curl);//clean curl resources
	}
}
