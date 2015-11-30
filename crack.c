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

char *curl_cmd = "curl ";


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
	FILE *ifp,*ofp;
	FILE *fp;
	char line[EMAIL_MAX_LENGTH];

	

	char certCheck[EMAIL_MAX_LENGTH];

	ifp = fopen("db.txt","r");
	if(ifp == NULL)
	{
		fclose(ifp);
		ofp = fopen("CertificateRepo.txt","r");
		while (fgets(line,sizeof(line)-1,ofp)!=NULL)
		{
			
			char unity[EMAIL_MAX_LENGTH];
			char url[EMAIL_MAX_LENGTH];
			
			int n;
			
			if(2 == sscanf(line, "%[^ ,],%[^ \n] %n", unity, url, &n) && line[n] == '\0')
			{
				//printf("%s",url);

				size_t length = strlen(unity);
				if(unity[length-1]=='\n')
				{
					//printf("Here");
					unity[length-1] = '\0';
				}
				//printf("%s",unity);
				//break;
				if(strcmp(email,unity)==0)
				{
					//printf("Here");

					fclose(ofp);

					char *unity_pem = concat(unity,".pem");

					char *cmd = concat(curl_cmd,url);
					cmd = concat(cmd," > ");
					cmd = concat(cmd,unity_pem);

					system(cmd);

					//printf("%s\n", cmd);

					char *verify_cert = concat(verify_cmd,unity_pem);

					fp = popen(verify_cert,"r");
					fgets(certCheck,sizeof(certCheck)-1,fp);
					pclose(fp);

					//printf("%s\n", certCheck);

					char *verify_cert_OK = concat(unity_pem,": OK\n");

					if(strcmp(certCheck,verify_cert_OK)==0)
					{
						ifp = fopen("db.txt","w");
						fprintf(ifp,"%s %s\n",unity, unity_pem);
						fclose(ifp);

						return unity_pem;
					}
				}
			
			 }
		}

	}

	char user[100];
	char cert[100];
	int not_found = 1;

	while (fscanf(ifp,"%s %[^\n]s", user,cert)!=EOF) {
  	//printf("%s %s", user,cert);
		if(strcmp(email,user)==0)
		{
			//printf("Here");
			//printf("%s",cert);
			not_found = 0;
			char *pem_file = cert;
			return pem_file;
		}
  }
  fclose(ifp);

  if(not_found == 1)
  {
  	ofp = fopen("CertificateRepo.txt","r");
		while (fgets(line,sizeof(line)-1,ofp)!=NULL)
		{
			
			char unity[EMAIL_MAX_LENGTH];
			char url[EMAIL_MAX_LENGTH];
			
			int n;
			
			if(2 == sscanf(line, "%[^ ,],%[^ \n] %n", unity, url, &n) && line[n] == '\0')
			{
				//printf("%s",url);

				size_t length = strlen(unity);
				if(unity[length-1]=='\n')
				{
					//printf("Here");
					unity[length-1] = '\0';
				}
				//printf("%s",unity);
				//break;
				if(strcmp(email,unity)==0)
				{
					//printf("Here");

					fclose(ofp);

					char *unity_pem = concat(unity,".pem");

					char *cmd = concat(curl_cmd,url);
					cmd = concat(cmd," > ");
					cmd = concat(cmd,unity_pem);

					system(cmd);

					//printf("%s\n", cmd);

					char *verify_cert = concat(verify_cmd,unity_pem);

					fp = popen(verify_cert,"r");
					fgets(certCheck,sizeof(certCheck)-1,fp);
					pclose(fp);

					//printf("%s\n", certCheck);

					char *verify_cert_OK = concat(unity_pem,": OK\n");

					if(strcmp(certCheck,verify_cert_OK)==0)
					{
						ifp = fopen("db.txt","a");
						fprintf(ifp,"%s %s\n",unity, unity_pem);
						fclose(ifp);

						return unity_pem;
					}
				}
			
			 }
		}

  }
	
}

void listDB()
{
	FILE *ifp;
	char line[EMAIL_MAX_LENGTH]; 
	char user[100];
	char cert[100];

	ifp = fopen("db.txt","r");
	if(ifp == NULL)
	{
		fprintf(stderr, "Can't open input file db.txt!\n");
		return;
	}
	// while (fgets(line,sizeof(line)-1,ifp)!=NULL) {
 //  	printf("%s", line);

  	while (fscanf(ifp,"%s %[^\n]s", user,cert)!=EOF) {
  	printf("%s %s\n", user,cert);
  }
  fclose(ifp);
}

void sendMSG()
{
	FILE *fp;
	printf("Enter destination email address and message\n");
    char get_email[EMAIL_MAX_LENGTH];
	char email[EMAIL_MAX_LENGTH];
	char msg[EMAIL_MAX_LENGTH];
	char certCheck[EMAIL_MAX_LENGTH];
	char randPwd[EMAIL_MAX_LENGTH];
	char enc_text[EMAIL_MAX_LENGTH];
	char enc_pwd_wPubkey[EMAIL_MAX_LENGTH];
	char signed_enc_text[EMAIL_MAX_LENGTH];
	scanf("%s",get_email);
	scanf(" %[^\n]s",msg);
	//printf("get email is %s",email);
    
    int k=0;
    for(int i=0;get_email[i]!='@';i++) {
        email[k++]=get_email[i];
    }
    
    //printf("email is %s",email);

	char *cert = getCert(email);
	//printf("%s",cert);

	char *verify_cert = concat(verify_cmd,cert);
	fp = popen(verify_cert,"r");
	fgets(certCheck,sizeof(certCheck)-1,fp);
	pclose(fp);


	char *verify_cert_OK = concat(cert,": OK\n");

	if(strcmp(certCheck,verify_cert_OK)==0)
	{
		fp = popen(randomPassword_cmd,"r");
		//fgets(randPwd,sizeof(randPwd)-1,fp);
		fscanf(fp, "%s", randPwd);
		pclose(fp);
		//printf("%s", randPwd);

		char *echo_msg = concat("echo ",msg);
		//char *echo_msg1 = concat(echo_msg,"\"");

		char *enc_cmd_noPwd = concat(echo_msg,enc_cmd);

		char *enc_cmd_wPwd = concat(enc_cmd_noPwd, randPwd);

		fp = popen(enc_cmd_wPwd,"r");
		//fgets(enc_text,sizeof(enc_text)-1,fp);
		fscanf(fp, "%s", enc_text);
		pclose(fp);
		//printf("%s\n", enc_text);

		char *publicKey = concat(cert, " > ");
		char *publicKey1 = concat(email, "_pub.pem");

		char *publicKey_extract = concat(publicKey,publicKey1);

		char *extract_pubKey1 = concat(extract_pubKey,publicKey_extract);
		system(extract_pubKey1);

		char *echo_ms = concat("echo ", randPwd);
		//printf("%s",echo_ms);

		char *enc_pwd_noPubkey = concat(echo_ms, enc_pwd_cmd);
		char *enc_pwd_wPubkey_cmd = concat(enc_pwd_noPubkey, publicKey1);
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

		//printf("%s",signed_enc_text_cmd);
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
		char *from_to = concat("from: sramakr6,",to_email); 
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

		printf("\n%s\n", send_msg);
        
        fclose(fp);



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
    
        //char *email = "sramakr6";
        char *sender_email = malloc(sizeof(char)*EMAIL_MAX_LENGTH);
        char *email = malloc(sizeof(char)*EMAIL_MAX_LENGTH);

        printf("Enter the sender's email address whose message is to be fetched\n");
		scanf("%s",sender_email);
    
        int k=0;
        for(int i=0;sender_email[i]!='@';i++) {
            email[k++]=sender_email[i];
        }
    
    
		char *cert = getCert(email);
		//printf("%s",cert);

		char *verify_cert = concat(verify_cmd,cert);

		fp = popen(verify_cert,"r");
		fgets(certCheck,sizeof(certCheck)-1,fp);
		pclose(fp);

		//printf("%s",certCheck);

		char line[EMAIL_MAX_LENGTH];

		char rcvd_msg[20][EMAIL_MAX_LENGTH];

		char *verify_cert_OK = concat(cert,": OK\n");

		if(strcmp(certCheck,verify_cert_OK)==0)
		{	
			
			char *publicKey = concat(cert, " > ");
			char *publicKey1 = concat(email, "_pub.pem");

			char *publicKey_extract = concat(publicKey,publicKey1);

			char *extract_pubKey1 = concat(extract_pubKey,publicKey_extract);
			//printf("%s",extract_pubKey1);
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

			char *from_header = rcvd_msg[0];
			//printf("%s", from_header);
			int count = 6;
			while(from_header[count]!=',')
			{
				printf("%c", from_header[count]);
				count++;
			}
			//printf("%d",count);
			// char sender[count-6+1];

			// memcpy(sender, &from_header[6], count-6);
			// sender[count-6] = '\0';

			// printf("%s",sender);


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

			char *verify_sha = concat(verify_sha_cmd, publicKey1);
			verify_sha = concat(verify_sha, " -signature ");
			verify_sha = concat(verify_sha, "signed_cont_cipher.sha1");
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
					listDB();
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