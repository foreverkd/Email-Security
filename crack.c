#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMAIL_MAX_LENGTH 1024
char *verify_cmd = "openssl verify -CAfile root-ca.crt ";
char *randomPassword_cmd = "openssl rand -base64 32";
char *enc_cmd = " | openssl enc -aes-256-cbc -base64 -pass pass:";
char *extract_pubKey = "openssl x509 -pubkey -in ";

char *enc_pwd_cmd =" | openssl rsautl -out file.bin -encrypt -pubin -inkey ";
char *sign_enc_text_cmd =" | openssl dgst -sha1 -sign private_key.pem -out signed.sha1";
//char *sign_enc_pwd_cmd ="openssl dgst -sha1 -sign private_key.pem -out enc_pwd.sha1 file.bin";
char *base64_enc_cmd = "openssl enc -base64 -in file.bin";
char *base64_dec_cmd = "openssl enc -base64 -d -in ";
char *base64_sign_cmd = "openssl enc -base64 -in signed.sha1";
char *verify_sha_cmd = "openssl dgst -sha1 -verify ";

char *send_msg=""; //final message
char *dec_session_key_cmd = "openssl rsautl -decrypt -inkey private_key.pem -in session_key.bin -passin pass:cookies";

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char * getCert(char *email)
{
	
}
//void listDB();
void sendMSG()
{
	FILE *fp;
	printf("Enter destination email address and message\n");
	char email[EMAIL_MAX_LENGTH];
	char msg[EMAIL_MAX_LENGTH];
	char certCheck[EMAIL_MAX_LENGTH];
	char randPwd[EMAIL_MAX_LENGTH];
	char enc_text[EMAIL_MAX_LENGTH];
	char enc_pwd_wPubkey[EMAIL_MAX_LENGTH];
	char signed_enc_text[EMAIL_MAX_LENGTH];
	scanf("%s",email);
	scanf(" %[^\n]s",msg);
	//printf("%s",email);
	char *verify_cert = concat(verify_cmd,"1021.pem");
	fp = popen(verify_cert,"r");
	fgets(certCheck,sizeof(certCheck)-1,fp);
	pclose(fp);

	if(strcmp(certCheck,"1021.pem: OK\n")==0)
	{
		fp = popen(randomPassword_cmd,"r");
		//fgets(randPwd,sizeof(randPwd)-1,fp);
		fscanf(fp, "%s", randPwd);
		pclose(fp);
		printf("%s", randPwd);

		char *echo_msg = concat("echo ",msg);
		//char *echo_msg1 = concat(echo_msg,"\"");

		char *enc_cmd_noPwd = concat(echo_msg,enc_cmd);

		char *enc_cmd_wPwd = concat(enc_cmd_noPwd, randPwd);

		fp = popen(enc_cmd_wPwd,"r");
		//fgets(enc_text,sizeof(enc_text)-1,fp);
		fscanf(fp, "%s", enc_text);
		pclose(fp);
		//printf("%s\n", enc_text);

		char *extract_pubKey1 = concat(extract_pubKey,"1021.pem > 1021_pub.pem");
		system(extract_pubKey1);

		char *echo_ms = concat("echo ", randPwd);
		//printf("%s",echo_ms);

		char *enc_pwd_noPubkey = concat(echo_ms, enc_pwd_cmd);
		char *enc_pwd_wPubkey_cmd = concat(enc_pwd_noPubkey, "1021_pub.pem");
		//printf("%s",enc_pwd_wPubkey);
		system(enc_pwd_wPubkey_cmd);

		char *enc_pwd="";
		fp = popen(base64_enc_cmd,"r");
		
		while (fgets(enc_pwd_wPubkey, sizeof(enc_pwd_wPubkey)-1, fp) != NULL) {
    		//size_t length = strlen(enc_pwd_wPubkey);
			//if (enc_pwd_wPubkey[length-1] == '\n')
			//{
    				//enc_pwd_wPubkey[length-1] = '\0';
					enc_pwd = concat(enc_pwd,enc_pwd_wPubkey);
			//}
  		}
		pclose(fp);
		//printf("%s",enc_pwd);

		char *signed_echo = concat("printf \"",enc_pwd);
		char *temp = concat(signed_echo,"\n");
		char *temp1 = concat(temp,enc_text);
		char *temp2 = concat(temp1,"\"");
		char *signed_enc_text_cmd = concat(temp2, sign_enc_text_cmd);

		printf("%s",signed_enc_text_cmd);
		system(signed_enc_text_cmd);
		//printf("%s",signed_enc_text_cmd);

		fp = popen(base64_sign_cmd,"r");

		char *signed_content = "";
		char buffer[EMAIL_MAX_LENGTH];
		while (fgets(buffer, sizeof(buffer)-1, fp) != NULL) {
    		//size_t length = strlen(buffer);
			//if (buffer[length-1] == '\n')
			//{
    		//		buffer[length-1] = '\0';
					signed_content = concat(signed_content,buffer);
			//}
  		}
  		pclose(fp);
		//printf("%s",signed_content);


		//Begin Formatting

		char *to_email = concat("to: ",email);
		char *from_to = concat("from: sramakr6@ncsu.edu,",to_email); 
		char *from_to_msg = concat(from_to,"\n"); //First Line

		char *begin_msg = concat(from_to_msg, "-----BEGIN CSC574 MESSAGE-----\n"); //Second line

		char *pwd_msg = concat(begin_msg,enc_pwd); //Third line

		char *empty_line = concat(pwd_msg,"\n"); //Fourth line

		char *enc_msg = concat(empty_line,enc_text); //Fifth line

		char *newline = concat(enc_msg,"\n\n"); //Sixth line

		char *signature = concat(newline,signed_content); //Seventh Line

		//char *newline2 = concat(signature, "\n");

		send_msg = concat(signature,"-----END CSC574 MESSAGE-----");

		// char *t = concat("printf \"",send_msg);
		// t = concat(t,"\" > send_msg.txt");
		// //printf("%s",t);
		// system(t);

		fp = fopen("send_msg.txt", "w");
		fprintf(fp,"%s",send_msg);

		printf("%s", send_msg);



	}

	//printf("%s",certCheck);
	//system(verify_cert);
	//system(randomPassword);
	//char *cert = getCert(email);
	//return;
}
void rcvMSG()
{
		FILE *fp;
		FILE *pp;
		char certCheck[EMAIL_MAX_LENGTH];

		char *verify_cert = concat(verify_cmd,"1021.pem");

		fp = popen(verify_cert,"r");
		fgets(certCheck,sizeof(certCheck)-1,fp);
		pclose(fp);

		char line[EMAIL_MAX_LENGTH];

		char rcvd_msg[15][EMAIL_MAX_LENGTH];

		if(strcmp(certCheck,"1021.pem: OK\n")==0)
		{	
			
			char *extract_pubKey1 = concat(extract_pubKey,"1021.pem > 1021_pub.pem");
			system(extract_pubKey1);

			fp = fopen("send_msg.txt","r"); 
			int i=0;
			while(fgets(line,sizeof(line),fp)) 
			{
				strcpy(rcvd_msg[i],line);
				//printf("Next line %s",rcvd_msg[i]);
				i++;
			}

			fclose(fp);

			char *content = rcvd_msg[2];
			int newlineflag=0;
			//printf("total %d lines", i);
			for(i=3;i<15;i++) 
			{
				if (strcmp(rcvd_msg[i],"\n") == 0 && newlineflag==1)
					break;
				if (strcmp(rcvd_msg[i],"\n") == 0)
					newlineflag++;
				content = concat(content,rcvd_msg[i]);
			}
			
			//char *content = concat(rcvd_msg[2], rcvd_msg[3]);
			//content = concat(content, rcvd_msg[4]);

			size_t length = strlen(content);
			if (content[length-1] == '\n') {
				
				content[length-1] = '\0';
			}

			//printf("%s",content);

			fp = fopen("content.txt", "w");
			fprintf(fp,"%s",content);
			fclose(fp);



			char *signed_content = rcvd_msg[i+1];
			int j;
			for(j=i+2;j<15;j++) 
			{
				if (strcmp(rcvd_msg[j],"-----END CSC574 MESSAGE-----") == 0)
					break;
				signed_content = concat(signed_content,rcvd_msg[j]);
			}


			length = strlen(signed_content);
			if (signed_content[length-1] == '\n') {
				
				signed_content[length-1] = '\0';
			}


			fp = fopen("signed_content.txt", "w");
			fprintf(fp,"%s",signed_content);
			fclose(fp);

			//printf("signed content %s",signed_content);	

			char *base64_dec = concat(base64_dec_cmd, "signed_content.txt");
			base64_dec = concat(base64_dec," -out signed_cont_cipher.sha1");

			//printf("%s",base64_dec);	
			system(base64_dec);

			char *verify_sha = concat(verify_sha_cmd, "1021_pub.pem");
			verify_sha = concat(verify_sha, " -signature ");
			verify_sha = concat(verify_sha, "signed_cont_cipher.sha1 ");
			verify_sha = concat(verify_sha, " content.txt");

			//printf("%s",verify_sha);

			char verify_res[EMAIL_MAX_LENGTH];
			pp = popen(verify_sha,"r");
			fgets(verify_res, sizeof(verify_res)-1, pp);
			pclose(pp);
			//printf("%s",verify_res);

			//system(verify_sha);

			if(strcmp(verify_res,"Verified OK\n")==0)
			{
				//printf("im here");
				char *session_key = rcvd_msg[2];
				for(i=3;i<15;i++) 
				{
					if (strcmp(rcvd_msg[i],"\n") == 0)
						break;	
					session_key = concat(session_key,rcvd_msg[i]);
				}

				length = strlen(session_key);
				if (session_key[length-1] == '\n') {
					session_key[length-1] = '\0';
				}

				fp = fopen("session_key.txt", "w");
				fprintf(fp,"%s",session_key);
				fclose(fp);

				char *session_key_dec = concat(base64_dec_cmd, "session_key.txt");
				session_key_dec = concat(session_key_dec," -out session_key.bin");

				//printf("%s",session_key_dec);

				system(session_key_dec);


				char dec_key[EMAIL_MAX_LENGTH];
				pp = popen(dec_session_key_cmd,"r");
				fscanf(pp,"%s",dec_key);
				pclose(pp);

				//printf("key %s\n",dec_key);

				char *dec_text = rcvd_msg[i+1];
				for(j=i+2;j<15;j++) 
				{
					if (strcmp(rcvd_msg[j],"\n") == 0)
						break;	
					dec_text = concat(dec_text,rcvd_msg[j]);
				}


				length = strlen(dec_text);
				if (dec_text[length-1] == '\n') {
					dec_text[length-1] = '\0';
				}

				//printf("%s",dec_text);

				char *dec_text_cmd = concat("echo ",dec_text);
				dec_text_cmd = concat(dec_text_cmd," | openssl enc -aes-256-cbc -d -base64 -pass pass:");
				dec_text_cmd = concat(dec_text_cmd,dec_key);

				//printf("%s",dec_text_cmd );
				system(dec_text_cmd);



			}
		}

}


int main()
{
	int n;
	while(1)
	{
		printf("1. List Database\n");
		printf("2. Send email\n");
		printf("3. Receive email\n");
		printf("4. Exit\n");
		scanf("%d",&n);

		switch(n)
		{
			case 1: 
					//listDB();
					break;
			case 2: 
					sendMSG();
					break;
			case 3: 
					rcvMSG();
					break;
			case 4: 
					exit(1);
					break;
			default: printf("Invalid choice\n");
					break;
		}
	}
	
}