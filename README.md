DRAGONFLY - a text-based game engine

Mark Claypool, WPI
Version 4.20, 2024

Additional information available at:

  http://dragonfly.wpi.edu/

PROJECT 2 README
LINK: https://drive.google.com/file/d/1Sch8PvvN-XCdEqreri_CBgWiWgU-ss98/view?usp=sharing

DISCLAIMER
I messaged the TA about the issue of my phone and laptop being stolen and having to restart during the weekend. Due to this I had to
rush and wasn't able to finish com

PROJECT STRUCTURE
yakserver.cpp: The server application that manages client connections and broadcasts messages.
Client.cpp / Client.h: The client-side implementation, which includes methods for connecting to the server, sending messages, and receiving messages to update the UI.
Input.cpp / Input.h: Handles user input and sends it to the server.
TextBox.cpp / TextBox.h: A UI component for displaying messages.
NetworkManager.cpp / NetworkManager.h: Manages network communication for both client and server.

HOW TO USE
On Microsoft Visual Studio, just fn+f5 for the client and server solution. First you run the server
solution and then the client solution.

Server (yakserver.cpp)
Listening for Connections: The server listens for incoming client connections on port 6000.
Handling Clients: Each client connection is handled in a separate thread. The server receives messages from clients and broadcasts them to all other connected clients.
Broadcasting Messages: The broadcastMessage function sends received messages to all connected clients.

Client
Connecting to Server: The client connects to the server and establishes a socket connection.
Sending Messages: The client sends messages to the server when the user inputs text.
Receiving Messages: The client continuously receives messages from the server and updates the UI to display them.



How It Works
Server (yakserver.cpp)
Listening for Connections: The server listens for incoming client connections on port 6000.
Handling Clients: Each client connection is handled in a separate thread. The server receives messages from clients and broadcasts them to all other connected clients.
Broadcasting Messages: The broadcastMessage function sends received messages to all connected clients.
Client
Connecting to Server: The client connects to the server and establishes a socket connection.
Sending Messages: The client sends messages to the server when the user inputs text.
Receiving Messages: The client continuously receives messages from the server and updates the UI to display them.
UI Components
TextBox: A UI component for displaying received messages. The TextBox class provides methods to add and display text.
Input: Handles user input and sends it to the server. It also updates the TextBox with the text entered by the user.

Issues
One issue is that the Clients dont display each other videos, but the server shows that
they both reach the server.

LICENSE

Dragonfly is free! This means free of charge for personal
use. Dragonfly may be copied and distributed, provided that this
copyright notice is kept intact. There cannot be a charge for the use
of Dragonfly to anyone else. The copyright holders (Mark Claypool and
WPI) reserve the right to reclassify this software as non-freeware at
a later date (e.g., as shareware). Doing so will not modify the
license agreement of any previously distributed executables.


REQUIREMENTS

Dragonfly requires SFML version 2.6 or later.

Dragonfly has been developed and tested on Linux, Microsoft Windows in
Visual Studio and Mac OS in Xcode and with homebrew.  Instructions for
setting up a development environment are provided online:

http://dragonfly.wpi.edu/engine/index.html#setup


NOTES ON DEVELOPING WITH DRAGONFLY

+ The engine must be run with a font file in the current directory.
The default is df-font.ttf.

+ The upper left corner of the game world (and view) is (0,0).
Positive values go horizontally right (x) and vertically down (y).

+ Generally, most Dragonfly functions return 0 if they are successful,
and return a non-zero number (usually -1) if there is a problem.  Most
problems are accompanied by a corresponding note in the logfile
(dragonfly.log).

+ Game objects are handled as pointers by Dragonfly.  Thus, upon
instantiating, game objects (inherited from Object) allocated (via
new) by the programmer's game code and then pointers to the objects
are passed into the WorldManager.  Objects *must* be added to the
world in this way if Dragonfly is to act upon them.

+ Game objects should not delete themselves or others, but should
rather add themselves to delete queue (via WorldManager
markForDelete()).  While ViewObjects can safely be deleted in a normal
fashion, they can also be deleted by markForDelete().

+ Objects do not need to register for collision events, but get these
automatically if they are solid Objects (they can, of course, be
ignored).  ViewObjects do not get collision events.

+ When an Object crosses from inside the game world (specified by
(x,y) not by the bounding box) to outside the game world (outside of
the WorldManager boundary) an "outofbounds" event is sent to the
object.  Note, this only happens once when and Object moves outside
and does repeat itself if it stays outside, nor is there any
corresponding event generated when an outside object comes in.
Objects do not need to register for "outofbounds" events, but get
these automatically (they can, of course, be ignored).

+ When there is a collision between two Objects, a collision event
object (EventCollision) is sent to *both* Objects.  The Object that
was moving (and caused the collision) is always returned via the
Object1() method and the object it hits via the Object2() method.

+ Custom events defined by the game programmer events that are of
interest via the Object::registerInterest() are added to the
WorldManager.  This means the WorldManager should be used for any
onEvent() calls to them.

+ Objects do not need to have sprites to display characters on the
screen.  They can override the draw() event and then manually draw (or
not) any characters they want on the screen using the
GraphicsManager::drawCh() method.

+ The scene graph is organized to make drawing and collision detection
in the game loop efficient.  The scene graph clusters Objects by their
visibility, solidness and altitude.  It assumes that changes to these
properties (e.g., changing altitude) are infrequent and is not designed
to be particularly efficient in doing so.  

+ Frames are colorless.  Object Sprite colors are defined in the
sprite file.  When loaded, the color then resides with the Sprite
object.  This means all frames in a Sprite are the same color.

+ The default font used by Dragonfly is Anonymous Pro (in the file
named df-font.ttf). See:
https://www.google.com/fonts/specimen/Anonymous+Pro

+ Sprite transparency can be enabled via the Sprite method
"setTransparency()".  When called with no arguments, it defaults to a
space (' '), but can be set to any character.  Transparent sprite
characters still cause collisions, but are not drawn.

+ Many game engine options can be changed in the configuration file,
df-config.txt.  Lines beginning with a "#" are comments and are
ignored.  The line order does not matter.

+ Custom game configuration parameters can be fairly easily managed in
the Dragonfly config file, df-config.txt, and the methods for the
Config singleton in "Config.h".  For example, configuration of the
amount of health can be set in the file:

  # Amount of health for the Hero.
  health:100,

and matched with:

  std::string s = df::Config::getInstance().match("health");
  if (s != "")
    int health = atoi(s.c_str());
