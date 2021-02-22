#include <ESP8266HTTPClient.h>   //webhook fire 
#include <ESP8266WiFi.h>   ///esp inbuilt
#include <FirebaseArduino.h>   //connect to firebase   
#include<NTPClient.h>  //NTP time server
#include<WiFiUdp.h>    //Support lib for ntp


// Set these to run example.
#define FIREBASE_HOST "nodemcu-6692b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "x9QtiGGCCAamGXrxll53ZwErbPYfKsg9uqbgafga"
#define WIFI_SSID "sbd"
#define WIFI_PASSWORD "Sagar@123"

String wifi = "Sagar@123";
const long utcOffsetInSeconds = 5*3600+1800;
String currenttime,uid;






//*******************************************************************************************************************************************************************************************//
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);




//*******************************************************************************************************************************************************************************************//
//Setup Programs//

void setup() {
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  timeClient.begin();
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
   //getting user id
    uid = Firebase.getString("users/"+wifi+"/uid");


   timeClient.begin(); 

}




//*******************************************************************************************************************************************************************************************//
//Variable Declaration//

int key;               //Encryption Key
int systolic_bp;       //Systolic bp A1-lower critical level A2-normal A3-upper critical level
int diastolic_bp;      //Diastolic bp
int hb;                //Hemoglobin
int glucose;           //Sugar Leval
int bodytemp;          //Temp Sensor
int BPM;               //Heart Rate


int countSBP=0,countDBP=0,countHB=0,countSugar=0,countTemp=0,countHRate=0,avg=0,value=0,isTime=0,avgSBP=0,avgDBP=0,avgHB=0,avgSugar=0,avgTemp=0,avgHRate=0;  ///Vriable of average program


//****************************************************************************************************************************************************************************************//
//Main Programs//

void loop() {

timeClient.update();
 unsigned long epochTime = timeClient.getEpochTime();
 struct tm *ptm = gmtime ((time_t *)&epochTime); 
  String formattedTime = timeClient.getFormattedTime(); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
   int currentSecond = timeClient.getSeconds();
  String Hour;
  (currentHour-12)>=10?Hour=String(currentHour-12):Hour="0"+String(currentHour-12);
  String Minute;
  (currentMinute)>=10?Minute=String(currentMinute):Minute="0"+String(currentMinute);
   String Second;
  (currentSecond)>=10?Second=String(currentSecond):Second="0"+String(currentSecond);
    String Day;
  (monthDay)>=10?Day=String(monthDay):Day="0"+String(monthDay);
   String Month;
  (currentMonth)>=10?Month=String(currentMonth):Month="0"+String(currentMonth);

  if(currentHour>12 && currentHour<24){
      currenttime=Hour+":"+Minute+":"+Second+" PM";
     }else if(currentHour<12){
      currenttime=Hour+":"+Minute+":"+Second+" AM";
     }else if(currentHour==24){
      currenttime=Hour+":"+Minute+":"+Second+" AM";
    }else if(currentHour==0){
      currenttime="12:"+Minute+":"+Second+" AM";
    }

  String currentDate = Day+ "-" + Month + "-" + String(currentYear);

  Serial.print("Current date: ");
  Serial.println(currentDate);
   Serial.print("Current time: ");
  Serial.println(currenttime);

 //make json object of data for sending in firebase
StaticJsonBuffer<1000> jsonBuffer;
JsonObject& data = jsonBuffer.createObject();
JsonObject& notify = jsonBuffer.createObject();

String timestamp = (String)millis();
//
//





  
  key = random(10);
  //Firebase.setInt("Hitman/A",key);
  Serial.print("Case ");
  Serial.println(key);


  systolic_bp = systolic_Critical_check();
  //Firebase.setInt("Hitman/B",systolic_bp);
  Serial.print("Systolic ");
  Serial.println(systolic_bp);



  diastolic_bp = diastolic_Critical_check();
  //Firebase.setInt("Hitman/C",diastolic_bp);
  Serial.print("Diastolic ");
  Serial.println(diastolic_bp);

  data["blood pressure"] = (String)systolic_bp+"/"+(String)diastolic_bp;



  hb = Hb_check();
  //Firebase.setInt("Hitman/D",hb);


  data["hb"] = (String)hb;


  glucose = sugar_check();
  //Firebase.setInt("Hitman/E",glucose);
  Serial.print("Glucose ");
  Serial.println(glucose);

  data["sugar"] = (String)glucose;


  bodytemp = Temp_check();
  //Firebase.setInt("Hitman/F",bodytemp);
data["temp"] = (String)bodytemp;


  BPM = HeartRate_check();
  //Firebase.setInt("Hitman/G",BPM);
 data["bps"] = (String)BPM;

  Firebase.set("hitman/"+uid+"/health/"+currentDate+"/"+currenttime+"/",data);

  checktimeAndSendMail();
  delay(30000);
}



//*******************************************************************************************************************************************************************************************//
//Encryption Programs//

double encryption(int key , double num) {
  //key = 10; ///for no encryption
  switch (key)
  {
    case 0:
      return num * num - num - 10;
      break;
    case 1:
      return num * num - num * 2 - 1;
      break;
    case 2:
      return num * num - num - 1;
      break;
    case 3:
      return num * 3 * num - num - 1;
      break;
    case 4:
      return 100 * num - num - 1;
      break;
    case 5:
      return num * num - num + 86;
      break;
    case 6:
      return num * num - num - 1 + num * 3;
      break;
    case 7:
      return num * num - num - 2;
      break;
    case 8:
      return num * num - num - 3;
      break;
    case 9:
      return num * num - num - 4;
      break;
    default:
      return num;
      break;
  }
}




//******************************************************************************************************************************************************************************************//
//Sensor and random value Programs//

int systolic() {
  return random(110, 120); //110 to 120

}


int diastolic() {
  return random(70, 80); //70 to 80
  
}


int Hb() {
  return random(13, 18);
}

int sugar() {
  return random(130, 140);

  
}

int HeartRate()
{
  return random(80, 90);
}


int Temp()
{
  double temp;
  double data = analogRead(A0);
  temp = log(10000.0 * ((1024.0 / data - 1)));
  temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp)) * temp);
  temp = temp - 273.15;
  return temp;
}




//*******************************************************************************************************************************************************************************************//
//Critical Check Programs//

int systolic_Critical_check()           ////Encryption + critical check(last digit) of systolic bp
{
  int systolic_bp = systolic();
  avgSBP=findAvg(systolic_bp,avgSBP*countSBP,countSBP+1);
  int result;
  String s3,s2,s1=(String)systolic_bp;
  
  if (systolic_bp >= 110 && systolic_bp < 120) {                 ///normal
    systolic_bp = encryption(key , systolic_bp);
     result = systolic_bp*10+0;
          result = result*10+key;
    
    return (result );
  };


  if (systolic_bp >= 120 && systolic_bp < 130) {                 ///Elevated Bp
    systolic_bp = encryption(key , systolic_bp);
   result = systolic_bp*10+1;
          result = result*10+key;    s2="Elevated+Blood+Pressure+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };

  if (systolic_bp >= 130 && systolic_bp < 140) {                 //////Hypertension stage 1  high BP
    systolic_bp = encryption(key , systolic_bp);
   result = systolic_bp*10+2;
          result = result*10+key;
          s2="Hypertension+Stage+1+(High+Blood+Pressure)+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };

  if (systolic_bp >= 140 && systolic_bp < 180) {                 ///Hypertension stage 2  High bp
    systolic_bp = encryption(key , systolic_bp);
   result = systolic_bp*10+3;
          result = result*10+key;    s2="Hypertension+Stage+2+(High+Blood+Pressure)+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };

  if (systolic_bp >= 180) {
    systolic_bp = encryption(key , systolic_bp);                 /////Highpertensive crisis
   result = systolic_bp*10+3;
          result = result*10+key;    s2="Highpertensive+crisis+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };
}

int diastolic_Critical_check()                                  //// Encryption + critical check(last digit) of diatolic bp
{
  int diastolic_bp = diastolic();
  avgDBP=findAvg(diastolic_bp,avgDBP*countDBP,countDBP+1);
  int result;
  String s3,s2,s1=(String)diastolic_bp;
  
  if (diastolic_bp >= 70 && diastolic_bp < 80) {                ///normal
    diastolic_bp = encryption(key , diastolic_bp);
   result = diastolic_bp*10+0;
          result = result*10+key;  
          return (result );
  };

  if (diastolic_bp >= 70 && diastolic_bp < 80) {                ///normal
    diastolic_bp = encryption(key , diastolic_bp);
   result = diastolic_bp*10+0;
          result = result*10+key;  
          return (result );
  };

  if (diastolic_bp >= 80 && diastolic_bp < 90) {               ///Hypertension stage 1  high BP
    diastolic_bp = encryption(key , diastolic_bp);
   result = diastolic_bp*10+1;
          result = result*10+key;  
          s2="Hypertension+Stage+1+(High+Blood+Pressure)+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };

  if (diastolic_bp >= 90 && diastolic_bp < 120) {              ///Hypertension stage 2  High bp
    diastolic_bp = encryption(key , diastolic_bp);
   result = diastolic_bp*10+2;
          result = result*10+key;  
          s2="Hypertension+Stage+2+(High+Blood+Pressure)+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };

  if (diastolic_bp >= 120) {                                    /////Highpertensive crisis
    diastolic_bp = encryption(key , diastolic_bp);
   result = diastolic_bp*10+3;
          result = result*10+key;  
          s2="Highpertensive+crisis+and+Your+Blood+Pressure+(BP)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  };
}


int Hb_check() {
  int hb = Hb();
  avgHB=findAvg(hb,avgHB*countHB,countHB+1);
  int result;
  String s3,s2,s1=(String)hb;

  if (hb < 13) {                             //////Anemia condition
    hb = encryption(key , hb);
   result = hb*10+1;
          result = result*10+key;  
          s2="Anemia+and+Your+blood+Heamoglobin+level+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }

  if (hb > 17) {                              //////////COPD & Pulmonory Fibrosis  (Lung diseases)
    hb = encryption(key , hb);
  result = hb*10+2;
  
          result = result*10+key;     s2="COPD+&+Pulmonory+Fibrosis+(Lung+diseases)+and+Your+blood+Heamoglobin+level+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }

  else {                                      /////////Normal range
    hb = encryption(key , hb);
  result = hb*10+0;
          result = result*10+key; 
          return (result);
  }
}


int sugar_check() {
  int Sugar = sugar();
  avgSugar=findAvg(Sugar,avgSugar*countSugar,countSugar+1);
  String s3,s2,s1=(String)Sugar;
  int result;

  if (Sugar < 140) {                        //////Normal
    Sugar = encryption(key , Sugar);
  result = Sugar*10+0;
          result = result*10+key; 
          return (result);
  }

  if (Sugar > 140 && Sugar < 200) {          //////prediabetise
    Sugar = encryption(key , Sugar);
  result = Sugar*10+1;
          result = result*10+key;  
          s2="Prediabetise+and+Your+blood+Sugar+level+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }

  if (Sugar > 200) {                         //////Diabetise
    Sugar = encryption(key , Sugar);
  result = Sugar*10+2;
          result = result*10+key; 
          s2="Diabetise+and+Your+blood+Sugar+level+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }
}


int HeartRate_check() {
  int Heart_Rate = HeartRate();               ////normal always
  avgHRate=findAvg(Heart_Rate,avgHRate*countHRate,countHRate+1);
  int result ;
  Heart_Rate = encryption(key , Heart_Rate);
  result = Heart_Rate*10+0;
          result = result*10+key; 
          return (result);
}

int Temp_check() {
  int temp = Temp();
  avgTemp=findAvg(temp,avgTemp*countTemp,countTemp+1);
  String s1=(String)temp;
  String s2,s3;
  int result;

  if (temp < 35.0) {                                   ///Hypothrmia (lower body Temp)
    temp = encryption(key , temp);
  result = temp*10+1;
          result = result*10+key; 
          //s2="Hypothrmia+(lower+body+Temp)+and+Temp('C)+is+";
    //s3=s2+s1;
   // data(s3);
    return (result);
  }

  if (temp > 35.0 && temp < 38.0) {                       ///Normal
    temp = encryption(key , temp);
  result = temp*10+0;
          result = result*10+key; 
          return (result);
  }

  if (temp > 38.0 && temp < 40.0) {                       ///Feaver (Higher Temp)
    temp = encryption(key , temp);
  result = temp*10+2;
          result = result*10+key; 
          s2="Feaver+(Higher Temp)+and+Your+body+Temp('C)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }
  if (temp >= 40.0) {                                   ///High Feaver (Too Higher Temp)
    temp = encryption(key , temp);
  result = temp*10+2;
          result = result*10+key; 
          s2="High+Feaver+(Too+Higher+Temp)+and+Your+body+Temp('C)+is+";
    s3=s2+s1;
    data(s3);
    return (result);
  }
}



//*******************************************************************************************************************************************************************************************//
//Sending Data to various Platform

void data (String data) {
  mail(data);                        ///send mail and sms when its critical
  SMS(data);
  return ;
}

int isSend=0;
void checktimeAndSendMail(){
  timeClient.update();               /// check Ntp server time and send mail and sms at 5pm
  int Hr=timeClient.getHours(); 
  if(Hr == 17 && isSend == 0)
  {
   String allData = getAllData();
   normalAleart(allData);
   clearAllData();
   isSend=1;      //to send alert only once at 5pm
  }
  if(Hr == 18)  
    isSend=0;
}

void normalAleart(String allData)
{
  normalMail(allData);       ////Normal aleart
  normalSMS;
  return;
}












//*******************************************************************************************************************************************************************************************//
// to do average of data



int findAvg(int val,int value,int count)
{
  //count=count+1;
  value=val+value;           //val=incomming data && value == previous addition data
  return value/count;
}

String getAllData()
{
  String s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15;
  s1=(String)avgSBP;
  s2=(String)avgDBP;
  s3=(String)avgHB;
  s4=(String)avgTemp;
  s5=(String)avgSugar;
  s6=(String)avgHRate;
  
  s7="<br>+Systolic+BP++++:";
  s8="<br>+Diastolic+BP+++:";
  s9="<br>+Heamoglobin++++:";
 s10="<br>+Body+Temp++++++:";
 s11="<br>+Sugar+Level++++:";
 s12="<br>+Heart+Rate+++++:";
 s14="+";
 s15=":";
  s13=s7+s14+s14+s14+s14+s15+s1   +s8+s14+s14+s14+s15+s2  +s9+s14+s14+s14+s14+s15+s3   +s10+s14+s14+s14+s14+s14+s14+s15+s4   +s11+s14+s14+s14+s14+s15+s5   +s12+s14+s14+s14+s14+s14+s15+s6;
  return s13;
}

void clearAllData()
{
  countSBP=0; countDBP=0; countTemp=0; countHB=0;countSugar=0;avg=0;value=0;isTime=0;avgSBP=0;avgDBP=0;avgHB=0;avgSugar=0;avgTemp=0;avgHRate=0; 
  return;
}













//*******************************************************************************************************************************************************************************************//
// Email Critical Alert


void mail(String state) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("http://maker.ifttt.com/trigger/mail/with/key/dEU-Hy1h_T70gsUQQCltOJX4c-QRPtj6Zf0ZYFyTBj1/?value1=" + state); //HTT
  int ret = 0;
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
}

void SMS(String state) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("http://maker.ifttt.com/trigger/SMS/with/key/dEU-Hy1h_T70gsUQQCltOJX4c-QRPtj6Zf0ZYFyTBj1/?value1=" + state); //HTT
  int ret = 0;
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
}




//*******************************************************************************************************************************************************************************************//
// normalEmail Alert

void normalSMS(String state) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("https://maker.ifttt.com/trigger/normalSMS/with/key/h2KBshBnM1WBWTHOEOIqLhPhgYlAW-VTBDWoLmUR6ws/?value1=" + state); //HTT
  int ret = 0;
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
}

void normalMail(String state) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("http://maker.ifttt.com/trigger/normalMail/with/key/h2KBshBnM1WBWTHOEOIqLhPhgYlAW-VTBDWoLmUR6ws/?value1=" + state); //HTTP
  int ret = 0;
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
}
