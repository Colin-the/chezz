from http.server import HTTPServer, BaseHTTPRequestHandler;

import urllib.parse
from urllib.parse import urlparse, parse_qsl;

import sys;
import os;
#import Physics #adding the libabry that we have made
import math
import json
import copy
import signal
import subprocess
import mimetypes
#import dataBase

#Commands to run the server on the right port on the socs server and see it on the browser
# python3 server.py 55313
# http://localhost:55313/shoot.html
# ./configure --enable-shared --with-sqlite3=/path/to/sqlite3

#http://localhost:8001/game.html

# ./configure
# make
# sudo make install

class MyServer(BaseHTTPRequestHandler):
    db = None; #to store our connection to the database
    loggedIn = False; #this will be set to true when the user is logged into an account
    usrEmail = None; #this will store the email of the account they are logged into

    def do_GET(self):
        if self.path.startswith("/getNameAndProfile"):
            try:
                if MyServer.db and MyServer.usrEmail:
                    usrData =MyServer.db.lookUpEmail(MyServer.db, MyServer.usrEmail);
                    if usrData:
                        user_record = usrData[0];
                        name = user_record[1];
                        pfp = user_record[-1];
                        response = {
                            "status": "success" if MyServer.loggedIn else "error",
                            "name": name,
                            "profilePicture": pfp
                        }
                        
                    else:
                        response = {
                            "status": "error",
                            "message": "User not found."
                        }
                else:
                    response = {
                        "status": "error",
                        "message": "User is not logged in or database connection is unavailable."
                    }


                json_response = json.dumps(response)
                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(json_response)))
                self.end_headers()
                self.wfile.write(json_response.encode('utf-8'))

            except Exception as e:
                # Handle server errors
                self.send_response(500)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                error_response = {
                    "status": "error",
                    "message": f"Internal server error: {str(e)}"
                }
                self.wfile.write(json.dumps(error_response).encode('utf-8'))

        if self.path.startswith("/getLoggedInStatus"):
            try:
                response = {
                    "status": "success" if MyServer.loggedIn else "error",
                    "message": "User is logged in" if MyServer.loggedIn else "User is not logged in"
                }
                
                json_response = json.dumps(response)

                # Send headers and response
                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(json_response)))
                self.end_headers()
                self.wfile.write(json_response.encode('utf-8'))

            except Exception as e:
                # Handle server errors
                self.send_response(500)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                error_response = {
                    "status": "error",
                    "message": f"Internal server error: {str(e)}"
                }
                self.wfile.write(json.dumps(error_response).encode('utf-8'))


        #If you need to use a image in one of your webpages put it in the images folder and this will automaticly handle the GET request for it
        # Just put the HTML <img src="Images/pin.png" alt="Image" class="image"> in your HTML file, put the image you want to use in the images folder
        #then update the path in the HTML tage to be src="Images/YOU_FILE_NAME_HERE" and this code will handle the rest
        if self.path.startswith("/images/"):
            file_path = self.path[1:]  # Remove the leading slash
            #print("Requested file:", file_path)

            if os.path.exists(file_path):
                # Get the content type
                content_type, _ = mimetypes.guess_type(file_path)
                
                # Read the file content
                with open(file_path, 'rb') as file:
                    file_content = file.read()

                # Send HTTP headers
                self.send_response(200)
                self.send_header("Content-type", content_type or "application/octet-stream")
                self.send_header("Content-length", len(file_content))
                self.end_headers()

                # Send the file content
                self.wfile.write(file_content)
            else:
                # File not found (no corrsponding image in the images folder)
                self.send_response(404)
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(b"File not found")


        # this one has its own request because there can't be a 404 for the home screen... that would be a bad start to the demo if it got 404'ed
        if self.path.startswith("/home.html"):
            with open("home.html", "r", encoding="utf-8") as file:
                homeHTML = file.read()

            self.send_response( 200 ); # OK
            self.send_header( "Content-type", "text/html" );
            self.send_header( "Content-length", str(len(homeHTML.encode("utf-8"))) );
            self.end_headers();

            self.wfile.write( bytes( homeHTML, "utf-8" ) ); #send the page over

        if self.path == "/login.html?accountCreated=true":
            #We need this GET request as this is what the Browser will request from the server when it has just created a new account
            #We will still be going to the login page however this lets us display a pop up message saying a new account has successfully been created when we get to the new page
            #so we just send over the login page in this case and the JS for login will handle the ?accountCreated=true
            with open("login.html", "r", encoding="utf-8") as file:
                loginPage = file.read()

            self.send_response( 200 ); # OK
            self.send_header( "Content-type", "text/html" );
            self.send_header( "Content-length", str(len(loginPage.encode("utf-8"))) );
            self.end_headers();
        
            self.wfile.write( bytes( loginPage, "utf-8" ) ); #send the page over
        

        elif self.path.endswith(".html"):
            if os.path.exists(self.path[1:]):
                with open(self.path[1:], "r", encoding="utf-8") as file:
                    htmlPage = file.read()
                #print("featching "+self.path[1:])
                self.send_response( 200 ); # OK
                self.send_header( "Content-type", "text/html" );
                self.send_header( "Content-length", str(len(htmlPage.encode("utf-8"))) );
                self.end_headers();

                self.wfile.write( bytes( htmlPage, "utf-8" ) ); #send the shoot page
            else:
                self.send_response(404)
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(b"File not found")


        #Defult responce to any js file request
        #the server will attempt to look in the local directory for the js file if it finds it then it will send it over
        # if the requsted file is not present then NO 404 will be sent as html pages should be the only thing using the js files  
        elif self.path.endswith(".js"):
            with open(self.path[1:], "r", encoding="utf-8") as file:
                gamePageJS = file.read()

            self.send_response( 200 ); # OK
            self.send_header( "Content-type", "text/javascript" );
            self.send_header("Content-length", str(len(gamePageJS.encode("utf-8"))))
            self.end_headers();

            self.wfile.write( bytes( gamePageJS, "utf-8" ) ); #send the shoot page


        #Defult responce to any css file request
        #the server will attempt to look in the local directory for the css file if it finds it then it will send it over
        # if the requsted file is not present then NO 404 will be sent as html pages should be the only thing using the css files  
        elif self.path.endswith(".css"):
            with open(self.path[1:], "r", encoding="utf-8") as file:
                gamePageStyles = file.read()
            print(self.path[1:])
            self.send_response( 200 ); # OK
            self.send_header( "Content-type", "text/css" );
            self.send_header( "Content-length", str(len(gamePageStyles.encode("utf-8"))) );
            self.end_headers();

            self.wfile.write( bytes( gamePageStyles, "utf-8" ) ); #send the shoot page

    
    def do_POST(self):
        if self.path == "/checkEmail":
            try:
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                content_type = self.headers.get('Content-Type')

                #print('Received POST data:', post_data)

                if content_type == 'application/json':
                    # Parse JSON data
                    data = json.loads(post_data)
                    # Only extract the email from the data (assuming the key is 'emailAddress')
                    email = data.get('emailAddress')

                elif content_type == 'application/x-www-form-urlencoded':
                    # Parse URL-encoded form data
                    data = urllib.parse.parse_qs(post_data.decode('utf-8'))
                    # Convert single-value lists to plain strings
                    data = {key: value[0] for key, value in data.items()}
                    # Only extract the email
                    email = data.get('emailAddress')

                else:
                    raise ValueError(f"Unsupported content type: {content_type}")

                #print('Parsed email:', email)
                
                #if there was somthing (presumed to be a email address) entered into the email feild of the create an account form
                if MyServer.db is not None:
                    try:
                        listOfAccounts = MyServer.db.lookUpEmail(MyServer.db, email)
                        #print("Successfully looked up email.")
                        #print("List of acc with same email: ",listOfAccounts)

                        if listOfAccounts == []:#if no other user in the DB has the email that the user just entered
                            #We can request the rest of the data from the browser and create an entry with the info in the db
                            #print("In no email found")
                            response = {
                                'status': 'success',
                                'message': 'Email is available for registration.'
                            }
                             
                            json_response = json.dumps(response)
                            self.send_response(200)  # OK
                            self.send_header("Content-Type", "application/json")
                            self.send_header("Content-Length", str(len(json_response)))
                            self.end_headers()
                            self.wfile.write(json_response.encode('utf-8'))
                        else:
                            #We have to reject the email as the email is alreay in use by another user which will create ishues down the line
                            response = {
                            'status': 'error',
                            'message': 'Looks like this email is already on file with our system and you already have an account!\nPlease try loging into the account with this email address if it is yours\nIf you need help reseting your password then reach out to our wonderful customer support team via email: communityFridgeExpansion@ouguelph.ca\nIf you were still having difficulties try creating a new account with a fresh email address'
                            }
                            json_response = json.dumps(response)
                            self.send_response(200)  # as this is NOT a server error we still respond OK and let the JS process the fact that the email address is currently in use by telling the user what to do
                            self.send_header("Content-Type", "application/json")
                            self.send_header("Content-Length", str(len(json_response)))
                            self.end_headers()
                            self.wfile.write(json_response.encode('utf-8'))
                            
                    except Exception as e:
                        print(f"Database lookup failed: {str(e)}")
                else:
                    print("No database connection available.")

                print("DOne email thingy")
                # self.send_response(200)  # OK
                # self.end_headers()
                # self.wfile.write(f'Email received: {email}'.encode())

                

            except json.JSONDecodeError:
                self.send_response(400)  # Bad Request
                self.end_headers()
                self.wfile.write(b'Invalid JSON format')

            except Exception as e:
                self.send_response(500)  # Internal Server Error
                self.end_headers()
                self.wfile.write(f'Internal server error: {str(e)}'.encode())
        elif self.path == "/registerAccountInDB":
            try:
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                content_type = self.headers.get('Content-Type')

                print('Received POST data in register acc:', post_data)

                if content_type == 'application/json':
                    
                    data = json.loads(post_data)# Parse JSON data
                    
                    #go threw and extract each feild from the json data into a python object
                    firstName = data.get('firstName')
                    lastName = data.get('lastName')
                    email = data.get('emailAddress')
                    password = data.get('password')
                    contact = data.get('contact')
                    profilePicture = data.get('profilePicture')


                    #As our database will treat this value as a 0/1 we need to convert it accordingly
                    if contact == 'yes':
                        contact = 1;
                    else:
                        contact = 0;
                

                    #now that weve extracted all of that info we can write it into the DB effectivly adding the users account to the system
                    MyServer.db.writeTable(MyServer.db, firstName, lastName, email, password, contact, profilePicture)

                    response = {
                                'status': 'success',
                                'message': 'Account successfully entered into the database'
                    }
                             
                    json_response = json.dumps(response)
                    self.send_response(200)  # OK
                    self.send_header("Content-Type", "application/json")
                    self.send_header("Content-Length", str(len(json_response)))
                    self.end_headers()
                    self.wfile.write(json_response.encode('utf-8'))
                    

                else:
                    raise ValueError(f"Unsupported content type: {content_type}")

            except json.JSONDecodeError:
                self.send_response(400)  # Bad Request
                self.end_headers()
                self.wfile.write(b'Invalid JSON format')

            except Exception as e:
                self.send_response(500)  # Internal Server Error
                self.end_headers()
                self.wfile.write(f'Internal server error: {str(e)}'.encode())

        elif self.path == "/logIntoAccount":
            try:
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                content_type = self.headers.get('Content-Type')

                print('Received POST data in register acc:', post_data)

                if content_type == 'application/json':
                    print("application")
                    data = json.loads(post_data)# Parse JSON data
                    
                    #go threw and extract each feild from the json data into a python object
                    email = data.get('emailAddress')
                    password = data.get('password')

                
                    print("e:",email," p: ",password)
                    #now that weve extracted all of that info we can write it into the DB effectivly adding the users account to the system
                    corrspondingAccount = MyServer.db.validLogin(MyServer.db, email, password)
                    print("From db: ",corrspondingAccount)
                    if corrspondingAccount != []:#If the username + password matched some user in our database then we will log them into that account
                        print("Found Combo in BD")
                        MyServer.loggedIn = True;
                        MyServer.usrEmail = email;
                        response = {
                                'status': 'success',
                                'message': 'Successfully logged into your account'
                        }
                                
                        json_response = json.dumps(response)
                        self.send_response(200)  # OK
                        self.send_header("Content-Type", "application/json")
                        self.send_header("Content-Length", str(len(json_response)))
                        self.end_headers()
                        self.wfile.write(json_response.encode('utf-8'))
                    else: #the email/password combo did not match anything in the database
                        print("No restund Found Combo in BD")
                        response = {
                                    'status': 'error',
                                    'message': 'Oops, the email/password combonation you entered do not match anything we have in our database.\n\nPlease try again and ensure you entered your email/password correctly'
                        }
                                
                        json_response = json.dumps(response)
                        self.send_response(200)  # OK
                        self.send_header("Content-Type", "application/json")
                        self.send_header("Content-Length", str(len(json_response)))
                        self.end_headers()
                        self.wfile.write(json_response.encode('utf-8'))
                    

                else:
                    raise ValueError(f"Unsupported content type: {content_type}")

            except json.JSONDecodeError:
                self.send_response(400)  # Bad Request
                self.end_headers()
                self.wfile.write(b'Invalid JSON format')

            except Exception as e:
                print("Unexpected error:", str(e))
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f'Internal server error: {str(e)}'.encode())

        
    # def send_response(self, code):
    #     # Set the HTTP response code
    #     self.send_header('Content-type', 'application/json')
    #     # Don't call send_response here, just set the headers and response code
    #     super().send_response(code)  # Use super() to call the base class's method
    #     self.end_headers()

    # def send_json(self, data):
    #     # Send the JSON data
    #     self.wfile.write(json.dumps(data).encode('utf-8'))

    

#server set up stuff
# httpd = HTTPServer( ( 'localhost', int(sys.argv[1]) ), MyServer );
# httpd.serve_forever();


def run(server_class=HTTPServer, handler_class=MyServer, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")

    #MyServer.db = dataBase.Database;#adding a connection to the database we made

    httpd.serve_forever()

def cleanSVGFolder (signum = None, frame = None, exit = True):
    #print("Signal received:", signum)   
    try:
        # Run the command to remove SVG files
        result = subprocess.run('rm tables/*.svg', shell=True, check=True)
        print("SVG files removed successfully.")
    except subprocess.CalledProcessError as e:
        print("No tables were saved")
    if (exit):
        sys.exit(0)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, cleanSVGFolder)  # Handle Ctrl+C
    run()
