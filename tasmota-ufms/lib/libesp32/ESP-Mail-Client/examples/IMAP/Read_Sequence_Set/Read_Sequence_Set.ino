/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP-Mail-Client
 *
 * Copyright (c) 2023 mobizt
*/

// This example shows how to read Email using UIDs ranges or message numbers ranges.

/** Note for library update from v2.x.x to v3.x.x.
 * 
 *  Struct data names changed
 *
 * "ESP_Mail_Session" changes to "Session_Config"
 * "IMAP_Config" changes to "IMAP_Data"
 *
 * Changes in the examples
 *
 * ESP_Mail_Session session;
 * to
 * Session_Config config;
 *
 * IMAP_Config config;
 * to
 * IMAP_Data imap_data;
 */

/** Assign SD card type and FS used in src/ESP_Mail_FS.h and
 * change the config for that card interfaces in src/extras/SDHelper.h
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <ESP_Mail_Client.h>

#define WIFI_SSID "<ssid>"
#define WIFI_PASSWORD "<password>"

/** For Gmail, IMAP option should be enabled. https://support.google.com/mail/answer/7126229?hl=en
 * and also https://accounts.google.com/b/0/DisplayUnlockCaptcha
 *
 * Some Gmail user still not able to sign in using account password even above options were set up,
 * for this case, use "App Password" to sign in instead.
 * About Gmail "App Password", go to https://support.google.com/accounts/answer/185833?hl=en
 *
 * For Yahoo mail, log in to your yahoo mail in web browser and generate app password by go to
 * https://login.yahoo.com/account/security/app-passwords/add/confirm?src=noSrc
 *
 * To use Gmai and Yahoo's App Password to sign in, define the AUTHOR_PASSWORD with your App Password
 * and AUTHOR_EMAIL with your account email.
 */

/* The imap host name e.g. imap.gmail.com for GMail or outlook.office365.com for Outlook */
#define IMAP_HOST "<host>"

/** The imap port e.g.
 * 143  or esp_mail_imap_port_143
 * 993 or esp_mail_imap_port_993
 */
#define IMAP_PORT 993

/* The log in credentials */
#define AUTHOR_EMAIL "<email>"
#define AUTHOR_PASSWORD "<password>"

/* Callback function to get the Email reading status */
void imapCallback(IMAP_Status status);

/* Print the list of mailbox folders */
void printAllMailboxesInfo(IMAPSession &imap);

/* Print the selected folder info */
void printSelectedMailboxInfo(SelectedFolderInfo sFolder);

/* Print all messages from the message list */
void printMessages(std::vector<IMAP_MSG_Item> &msgItems, bool headerOnly);

/* Print all attachments info from the message */
void printAttacements(std::vector<IMAP_Attach_Item> &atts);

/* Declare the Session_Config for user defined session credentials */
Session_Config config;

/* Declare the global used IMAPSession object for IMAP transport */
IMAPSession imap;

/* Define the IMAP_Data object used for user defined IMAP operating options. */
IMAP_Data imap_data;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

void readEmailsUIDS()
{

    int uid_last = imap.getUID(imap.selectedFolder().msgCount());
    int uid_begin = uid_last > 10 ? uid_last - 10 : uid_last;

    // Fetch last 10 UIDs
    String sequence_set = String(uid_begin) + ":" + String(uid_last);

    imap_data.fetch.sequence_set.string = sequence_set;

    imap_data.fetch.sequence_set.UID = true; // The sequence set are UIDs ranges

    imap_data.fetch.sequence_set.headerOnly = true; // Do not fetch the content, header only

    MailClient.readMail(&imap, false /* do not close sessiopn */);

    imap_data.limit.fetch = 5; // Set the limit of number of messages in the fetch results
}

void readEmailsNumbers()
{
    int msg_last = imap.selectedFolder().msgCount();
    int msg_begin = msg_last > 10 ? msg_last - 10 : msg_last;

    String sequence_set = String(msg_begin) + ":" + String(msg_last);

    imap_data.fetch.sequence_set.string = sequence_set;

    imap_data.fetch.sequence_set.UID = false; // The sequence set are not UIDs

    imap_data.fetch.sequence_set.headerOnly = true; // Do not fetch the content, header only

    imap_data.limit.fetch = 5; // Set the limit of number of messages in the fetch results

    // Read Email and close session
    MailClient.readMail(&imap);

    /* Clear all stored data in IMAPSession object */
    imap.empty();
}

void setup()
{

    Serial.begin(115200);

#if defined(ARDUINO_ARCH_SAMD)
    while (!Serial)
        ;
#endif

    Serial.println();

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    multi.addAP(WIFI_SSID, WIFI_PASSWORD);
    multi.run();
#else
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

    Serial.print("Connecting to Wi-Fi");
        
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    unsigned long ms = millis();
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
        if (millis() - ms > 10000)
            break;
#endif
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    /*  Set the network reconnection option */
    MailClient.networkReconnect(true);

    // The WiFi credentials are required for Pico W
    // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    MailClient.clearAP();
    MailClient.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif

    /** Enable the debug via Serial port
     * 0 for no debugging
     * 1 for basic level debugging
     *
     * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
     */
    imap.debug(1);

    /* Set the callback function to get the reading results */
    imap.callback(imapCallback);

    /** In case the SD card/adapter was used for the file storagge, the SPI pins can be configure from
     * MailClient.sdBegin function which may be different for ESP32 and ESP8266
     * For ESP32, assign all of SPI pins
     * MailClient.sdBegin(14,2,15,13)
     * Which SCK = 14, MISO = 2, MOSI = 15 and SS = 13
     * And for ESP8266, assign the CS pins of SPI port
     * MailClient.sdBegin(15)
     * Which pin 15 is the CS pin of SD card adapter
     */

    /* Set the session config */
    config.server.host_name = IMAP_HOST;
    config.server.port = IMAP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;

    /* Set the storage to save the downloaded files and attachments */
    imap_data.storage.saved_path = F("/email_data");

    /** Set to enable the results i.e. html and text messaeges
     * which the content stored in the IMAPSession object is limited
     * by the option imap_data.limit.msg_size.
     * The whole message can be download through imap_data.download.text
     * or imap_data.download.html which not depends on these enable options.
     */
    imap_data.enable.html = true;
    imap_data.enable.text = true;

    /* Set to enable the sort the result by message UID in the decending order */
    imap_data.enable.recent_sort = true;

    /* Set to report the download progress via the default serial port */
    imap_data.enable.download_status = true;

    /* Header fields parsing is case insensitive by default to avoid uppercase header in some server e.g. iCloud
    , to allow case sensitive parse, uncomment below line*/
    // imap_data.enable.header_case_sensitive = true;

    /** Set the maximum size of message stored in
     * IMAPSession object in byte
     */
    imap_data.limit.msg_size = 512;

    /** Set the maximum attachments and inline images files size
     * that can be downloaded in byte.
     * The file which its size is largger than this limit may be saved
     * as truncated file.
     */
    imap_data.limit.attachment_size = 1024 * 1024 * 5;

    // If ID extension was supported by IMAP server, assign the client identification
    // name, version, vendor, os, os_version, support_url, address, command, arguments, environment
    // Server ID can be optained from imap.serverID() after calling imap.connect and imap.id.

    // imap_data.identification.name = "User";
    // imap_data.identification.version = "1.0";

    /* Connect to the server */
    if (!imap.connect(&config, &imap_data))
        return;

    if (imap.isAuthenticated())
        Serial.println("\nSuccessfully logged in.");
    else
        Serial.println("\nConnected with no Auth.");

    // Client identification can be sent to server later with
    /**
     * IMAP_Identification iden;
     * iden.name = "user";
     * iden.version = "1.0";
     *
     * if (imap.id(&iden))
     * {
     *    Serial.println("\nSend Identification success");
     *    Serial.println(imap.serverID());
     * }
     * else
     *    MailClient.printf("nIdentification sending error, Error Code: %d, Reason: %s", imap.errorCode(), imap.errorReason().c_str());
     */

    /*  {Optional} */
    printAllMailboxesInfo(imap);

    /* Open or select the mailbox folder to read or search the message */
    if (!imap.selectFolder(F("INBOX")))
        return;

    /*  {Optional} */
    printSelectedMailboxInfo(imap.selectedFolder());

    Serial.println("::::::::::::::::: Reading Emails using the UIDs ranges ::::::::::::::::");
    readEmailsUIDS();

    Serial.println("::::::::::: Reading Emails using the message numbers ranges :::::::::::");
    readEmailsNumbers();
}

void loop()
{
}

/* Callback function to get the Email reading status */
void imapCallback(IMAP_Status status)
{
    /* Print the current status */
    Serial.println(status.info());

    /* Show the result when reading finished */
    if (status.success())
    {
        /* Print the result */
        /* Get the message list from the message list data */
        IMAP_MSG_List msgList = imap.data();
        printMessages(msgList.msgItems, imap.headerOnly());

        /* Clear all stored data in IMAPSession object */
        imap.empty();
    }
}

void printAllMailboxesInfo(IMAPSession &imap)
{
    /* Declare the folder collection class to get the list of mailbox folders */
    FoldersCollection folders;

    /* Get the mailbox folders */
    if (imap.getFolders(folders))
    {
        for (size_t i = 0; i < folders.size(); i++)
        {
            /* Iterate each folder info using the  folder info item data */
            FolderInfo folderInfo = folders.info(i);
            MailClient.printf("%s%s%s", i == 0 ? "\nAvailable folders: " : ", ", folderInfo.name, i == folders.size() - 1 ? "\n" : "");
        }
    }
}

void printSelectedMailboxInfo(SelectedFolderInfo sFolder)
{
    /* Show the mailbox info */
    MailClient.printf("\nInfo of the selected folder\nTotal Messages: %d\n", sFolder.msgCount());
    MailClient.printf("UID Validity: %d\n", sFolder.uidValidity());
    MailClient.printf("Predicted next UID: %d\n", sFolder.nextUID());
    if (sFolder.unseenIndex() > 0)
        MailClient.printf("First Unseen Message Number: %d\n", sFolder.unseenIndex());
    else
        MailClient.printf("Unseen Messages: No\n");
        
    if (sFolder.modSeqSupported())
        MailClient.printf("Highest Modification Sequence: %llu\n", sFolder.highestModSeq());
    for (size_t i = 0; i < sFolder.flagCount(); i++)
        MailClient.printf("%s%s%s", i == 0 ? "Flags: " : ", ", sFolder.flag(i).c_str(), i == sFolder.flagCount() - 1 ? "\n" : "");

    if (sFolder.flagCount(true))
    {
        for (size_t i = 0; i < sFolder.flagCount(true); i++)
            MailClient.printf("%s%s%s", i == 0 ? "Permanent Flags: " : ", ", sFolder.flag(i, true).c_str(), i == sFolder.flagCount(true) - 1 ? "\n" : "");
    }
}

void printAttacements(std::vector<IMAP_Attach_Item> &atts)
{
    MailClient.printf("Attachment: %d file(s)\n****************************\n", atts.size());
    for (size_t j = 0; j < atts.size(); j++)
    {
        IMAP_Attach_Item att = atts[j];
        /** att.type can be
         * esp_mail_att_type_none or 0
         * esp_mail_att_type_attachment or 1
         * esp_mail_att_type_inline or 2
         */
        MailClient.printf("%d. Filename: %s, Name: %s, Size: %d, MIME: %s, Type: %s, Description: %s, Creation Date: %s\n", j + 1, att.filename, att.name, att.size, att.mime, att.type == esp_mail_att_type_attachment ? "attachment" : "inline", att.description, att.creationDate);
    }
    Serial.println();
}

void printMessages(std::vector<IMAP_MSG_Item> &msgItems, bool headerOnly)
{

    /** In devices other than ESP8266 and ESP32, if SD card was chosen as filestorage and
     * the standard SD.h library included in ESP_Mail_FS.h, files will be renamed due to long filename
     * (> 13 characters) is not support in the SD.h library.
     * To show how its original file name, use imap.fileList().
     */
    // Serial.println(imap.fileList());

    for (size_t i = 0; i < msgItems.size(); i++)
    {

        /* Iterate to get each message data through the message item data */
        IMAP_MSG_Item msg = msgItems[i];

        Serial.println("****************************");
        MailClient.printf("Number: %d\n", msg.msgNo);
        MailClient.printf("UID: %d\n", msg.UID);
        
        // The attachment status in search may be true in case the "multipart/mixed"
        // content type header was set with no real attachtment included.
        MailClient.printf("Attachment: %s\n", msg.hasAttachment ? "yes" : "no");

        MailClient.printf("Messsage-ID: %s\n", msg.ID);

        if (strlen(msg.flags))
            MailClient.printf("Flags: %s\n", msg.flags);
        if (strlen(msg.acceptLang))
            MailClient.printf("Accept Language: %s\n", msg.acceptLang);
        if (strlen(msg.contentLang))
            MailClient.printf("Content Language: %s\n", msg.contentLang);
        if (strlen(msg.from))
            MailClient.printf("From: %s\n", msg.from);
        if (strlen(msg.sender))
            MailClient.printf("Sender: %s\n", msg.sender);
        if (strlen(msg.to))
            MailClient.printf("To: %s\n", msg.to);
        if (strlen(msg.cc))
            MailClient.printf("CC: %s\n", msg.cc);
         if (strlen(msg.bcc))
            MailClient.printf("BCC: %s\n", msg.bcc);
        if (strlen(msg.date))
        {
            MailClient.printf("Date: %s\n", msg.date);
            MailClient.printf("Timestamp: %d\n", (int)MailClient.Time.getTimestamp(msg.date));
        }
        if (strlen(msg.subject))
            MailClient.printf("Subject: %s\n", msg.subject);
        if (strlen(msg.reply_to))
            MailClient.printf("Reply-To: %s\n", msg.reply_to);
        if (strlen(msg.return_path))
            MailClient.printf("Return-Path: %s\n", msg.return_path);
        if (strlen(msg.in_reply_to))
            MailClient.printf("In-Reply-To: %s\n", msg.in_reply_to);
        if (strlen(msg.references))
            MailClient.printf("References: %s\n", msg.references);
        if (strlen(msg.comments))
            MailClient.printf("Comments: %s\n", msg.comments);
        if (strlen(msg.keywords))
            MailClient.printf("Keywords: %s\n", msg.keywords);

        /* If the result contains the message info (Fetch mode) */
        if (!headerOnly)
        {
            if (strlen(msg.text.content))
                MailClient.printf("Text Message: %s\n", msg.text.content);
            if (strlen(msg.text.charSet))
                MailClient.printf("Text Message Charset: %s\n", msg.text.charSet);
            if (strlen(msg.text.transfer_encoding))
                MailClient.printf("Text Message Transfer Encoding: %s\n", msg.text.transfer_encoding);
            if (strlen(msg.html.content))
                MailClient.printf("HTML Message: %s\n", msg.html.content);
            if (strlen(msg.html.charSet))
                MailClient.printf("HTML Message Charset: %s\n", msg.html.charSet);
            if (strlen(msg.html.transfer_encoding))
                MailClient.printf("HTML Message Transfer Encoding: %s\n\n", msg.html.transfer_encoding);

            if (msg.rfc822.size() > 0)
            {
                MailClient.printf("\r\nRFC822 Messages: %d message(s)\n****************************\n", msg.rfc822.size());
                printMessages(msg.rfc822, headerOnly);
            }

            if (msg.attachments.size() > 0)
                printAttacements(msg.attachments);
        }

        Serial.println();
    }
}
