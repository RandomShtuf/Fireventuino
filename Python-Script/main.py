# import dependencies
import requests
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# info for recieving thingspeak data
CHANNEL_ID = '<CHANNEL ID>'

url = f'https://api.thingspeak.com/channels/{CHANNEL_ID}/feeds.json'
field1_url = f'https://api.thingspeak.com/channels/{CHANNEL_ID}/fields/1/last.json'
field2_url = f'https://api.thingspeak.com/channels/{CHANNEL_ID}/fields/2/last.json'

# info for sending emails
smtp_port = 587
smtp_server = "smtp.gmail.com"
email_from = "fireventuino@gmail.com"
email_list = ["<List of E-Mail recipients>"]
pswd = "<Key Provided by Gmail for Gmail Access>"
subject = "Fireventuino Alerts"

# function for reading thingspeak data
def get_field_values():
    response = requests.get(url)
    response1 = requests.get(field1_url)
    response2 = requests.get(field2_url)

    if response.status_code == 200:
        data1, data2 = (response1.json(), response2.json())
        
        # Accessing the last value of the field
        gas_value = data1['field1']
        temp_value = data2['field2']

        return gas_value, temp_value        

    else:
        print('Error occurred while retrieving data')

# function for sending emails
def send_emails(email_list, flt_gas, flt_temp):

    for person in email_list:

        body = f"""
        SMOKE DETECTED!!!
        gas sensor reads: {flt_gas}
        thermometer reads: {flt_temp}
        """

        msg = MIMEMultipart()
        msg['From'] = email_from
        msg['To'] = person
        msg['Subject'] = subject

        msg.attach(MIMEText(body, 'plain'))

        text = msg.as_string()

        print("Connecting to server...")
        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()
        server.login(email_from, pswd)
        print("Succesfully connected to server")
        print()

        print(f"Sending email to: {person}...")
        server.sendmail(email_from, person, text)
        print(f"Email sent to: {person}")
        print()

    # Close the port
    server.quit()

while True:
    gas_value, temp_value = get_field_values()

    print('gas:', gas_value)
    print('temperature:', temp_value)

    int_gas = int(gas_value)
    flt_temp = float(temp_value)

    # send email when smoke is detected
    if int_gas > 60:
        send_emails(email_list, int_gas, flt_temp)