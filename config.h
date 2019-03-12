/*
 * - ID_PAGE: page to store the key id (front door uses page 4)
 * - KEY_PAGE: page that stores the keyA for the id page (front door uses 7)
 */

#define ID_PAGE (4)
#define KEY_PAGE (7)


// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 1, 1);

#define SERVER_URL "makey.localdomain"
#define SERVER_PORT (80)

//comment out to use serial instead
#define DISPLAY_LCD

#define DOOR_PIN (4)

#define PN532_IRQ   (2)
#define PN532_RESET (3)
