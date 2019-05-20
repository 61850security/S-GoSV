# S-GoSV

Instructions for running S-GoSV with AEAD varients:

A.1. EtM_send.c:

EtM_send.c program constructs an ethernet frame with all required fields of the frame such as destination address, source address, ether type followed by GOOSE (Generic Object-Oriented Substation Event) Protocol Data Unit (PDU) fields. The PDU fields includes APPID (Application ID), Length of PDU, Reserved1, Reserved2, GOOSE Application Protocol Data Unit (APDU) fields followed by Frame Check Sequence (FCS) field. Reserved1 field consists of the length of extension field if the frame is protected with digital signature algorithm. Reserved2 field holds Cyclic Redundancy Check (CRC) value. GOOSE APDU consists of fields in Tag-Length-Value (TLV) format. 
After constructing GOOSE ethernet frame, EtM_send.c program encrypts GOOSE APDU with Advanced Encryption Standard (AES-128) encryption.  The size of the GOOSE APDU fields is 137 bytes. As AES-128 encrypts 16 bytes of data at a time, the size of GOOSE APDU is expanded to 144 bytes by padding. The result of AES-128 is cipher text which is further given as input to HMAC-SHA-256 (HMAC ()) MAC generation function that gives 32 bytes of Message Authentication Code (MAC) value. HMAC () function is defined in openSSL header files which was included. The generated MAC value is stored in an extended fields of GOOSE APDU.  Extension fields consists of version, time_of_Current_key1, time_of_Next_key2, keyID followed by 32-byte MAC value. 
EtM_send.c program construct the GOOSE PDU frame in send_buf [ ] character array for every one second and send it into the network.  The encryption time and MAC generation times are captured using clock () function defined in sys/times.h header file.  
Execution steps:

1)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
2)	Replace the destination and source MAC addresses with your intended MAC addresses in  
            EtM_send.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
3)	Replace the network interface name with your computer’s network interface name in  
            EtM_send.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

4)	compile and run the program  
            → To run EtM_send.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o EtM_send EtM_send.c -L usr/local/bin -lssl -lcrypto  
            #./EtM_send

A.2. EtM_recv.c:

EtM_recv.c program receives data from the sender program and extracts cipher text and MAC value from the received data (buf [ ] array) into cipher [ ] and hash_recv [ ] arrays respectively. Further, EtM_recv.c program generates new MAC value from cipher text using HMAC-SHA-256 (HMAC ()) MAC generation function and compares with received MAC value in hash_recv [ ] array. If both are matched, then the cipher text will be decrypted other wise the packet will not be processed further. The MAC generation and decryption times were captured using clock ( ) function defined in sys/times.h header file.  
Execution steps:
5)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
6)	Replace the destination MAC address with your receiver computer’s MAC address in  
            EtM_recv.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
7)	Replace the network interface name with your computer’s network interface name in  
            EtM_recv.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

8)	compile and run the program  
            → To run EtM_recv.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o EtM_recv EtM_recv.c -L usr/local/bin -lssl -lcrypto  
            #./EtM_recv




B.1. EandM_send.c:

EandM_send.c program constructs an ethernet frame with all required fields of the frame such as destination address, source address, ether type followed by GOOSE (Generic Object-Oriented Substation Event) Protocol Data Unit (PDU) fields. The PDU fields includes APPID (Application ID), Length of PDU, Reserved1, Reserved2, GOOSE Application Protocol Data Unit (APDU) fields followed by Frame Check Sequence (FCS) field. Reserved1 field consists of the length of extension field if the frame is protected with digital signature algorithm. Reserved2 field holds Cyclic Redundancy Check (CRC) value. GOOSE APDU consists of fields in Tag-Length-Value (TLV) format. 
After constructing GOOSE ethernet frame, EandM_send.c program encrypts GOOSE APDU with Advanced Encryption Standard (AES-128) encryption.  The size of the GOOSE APDU fields is 137 bytes. As AES-128 encrypts 16 bytes of data at a time, the size of GOOSE APDU is expanded to 144 bytes by padding. Further, GOOSE APDU fields of 137 bytes are given as input to HMAC-SHA-256 (HMAC ()) MAC generation function that gives 32 bytes of Message Authentication Code (MAC) value. HMAC () function is defined in openSSL header files which was included in header section of the program. The generated MAC value is stored in an extended fields of GOOSE APDU.  Extension fields consists of version, time_of_Current_key1, time_of_Next_key2, keyID followed by 32-byte MAC value. 
EandM_send.c program construct the GOOSE PDU frame in send_buf [ ] character array for every one second and send it into the network.  The encryption time and MAC generation times are captured using clock ( ) function defined in sys/times.h header file.  
Execution steps:
1)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
2)	Replace the destination and source MAC addresses with your intended MAC addresses in  
            EandM_send.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
3)	Replace the network interface name with your computer’s network interface name in  
            EandM_send.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

4)	compile and run the program  
            → To run EandM_send.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o EandM_send EandM_send.c -L usr/local/bin -lssl -lcrypto  
            #./EandM_send

B.2. EandM_recv.c:

EandM_recv.c program receives data from the sender program and extracts cipher text and MAC value from the received data (buf [ ] array) into cipher [ ] and hash_recv [ ] arrays respectively. Further, EandM_recv.c program decrypts GOOSE APDU data into plaintext and generates new MAC value from the decrypted plaintext using HMAC-SHA-256 (HMAC ( )) MAC generation function and compares with received MAC value in hash_recv[ ] array. If both are matched, then the received data is accepted otherwise the packet will not be processed further. The decryption and MAC generation times were captured using clock ( ) function defined in sys/times.h header file.  

Execution steps:
5)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
6)	Replace the destination MAC address with your receiver computer’s MAC address in  
            EandM_recv.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
7)	Replace the network interface name with your computer’s network interface name in  
            EandM_recv.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

8)	compile and run the program  
            → To run EandM_recv.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o EandM_recv EandM_recv.c -L usr/local/bin -lssl -lcrypto  
            #./EandM_recv




C.1. MtE_send.c:

MtE_send.c program constructs an ethernet frame with all required fields of the frame such as destination address, source address, ether type followed by GOOSE (Generic Object-Oriented Substation Event) Protocol Data Unit (PDU) fields. The PDU fields includes APPID (Application ID), Length of PDU, Reserved1, Reserved2, GOOSE Application Protocol Data Unit (APDU) fields followed by Frame Check Sequence (FCS) field. Reserved1 field consists of the length of extension field if the frame is protected with digital signature algorithm. Reserved2 field holds Cyclic Redundancy Check (CRC) value. GOOSE APDU consists of fields in Tag-Length-Value (TLV) format. 
	After constructing GOOSE ethernet frame, MtE_send.c program generates MAC value for GOOSE APDU with size 137 bytes using MAC generation function HMAC-SHA-256 (HMAC ( ) ). HMAC () function is defined in openSSL header files which was included in header section of the program. This gives 32 bytes of MAC value. The generated MAC value is stored in an extended fields of GOOSE APDU.  Extension fields consists of version, time_of_Current_key1, time_of_Next_key2, keyID followed by 32-byte MAC value. As AES-128 encrypts 16 bytes of data at a time, the size of GOOSE APDU is expanded to 144 bytes by padding The extended GOOSE APDU along with extended fields of GOOSE APDU is encrypted using AES-128 (AESEncryption ( ) ) function. 
MtE_send.c program construct the GOOSE PDU frame in send_buf [ ] character array for every one second and send it into the network.  The MAC generation time and encryption times are captured using clock ( ) function defined in sys/times.h header file.  

Execution steps:
1)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
2)	Replace the destination and source MAC addresses with your intended MAC addresses in  
            MtE_send.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
3)	Replace the network interface name with your computer’s network interface name in  
            MtE_send.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

4)	compile and run the program  
            → To run MtE_send.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o MtE_send MtE_send.c -L usr/local/bin -lssl -lcrypto  
            #./MtE_send

C.2. MtE_recv.c:

MtE_recv.c program receives data from the sender program and decrypts it to extracts GOOSE APDU data and MAC value from the received data (buf [ ] array) into plaintext [ ] and hash_recv [ ] arrays respectively. Further, MtE_recv.c program generates new MAC value from the GOOSE APDU data using HMAC-SHA-256 (HMAC ( )) MAC generation function and compares with received MAC value in hash_recv[ ] array. If both are matched, then the received data is accepted otherwise the packet will not be processed further. The decryption and MAC generation times were captured using clock ( ) function defined in sys/times.h header file.  

Execution steps:
5)	Install libssl library, if already installed ignore the following command. 
           #sudo apt-get install libssl-dev
	   
6)	Replace the destination MAC address with your receiver computer’s MAC address in  
            MtE_recv.c program.
            In the case of broadcast use 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF as destination. 
	    
7)	Replace the network interface name with your computer’s network interface name in  
            MtE_recv.c program. 
Run the ifconfig command at terminal to know the MAC address as well as interface name

8)	compile and run the program  
            → To run MtE_recv.c in Linux terminal.
            #sudo bash
            [sudo] password for Linuxuser:
            <enter password>
            #gcc -o MtE_recv MtE_recv.c -L usr/local/bin -lssl -lcrypto  
            #./MtE_recv
