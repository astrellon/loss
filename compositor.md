Compositing
===========

Currently looking at a client/server structure. Partially because of precident from X and Wayland but also because it should offer the most flexibility which in a system designed to be embedded into other systems, this seems appropriate.

Components
----------

- Output Screens
- Windows
- Desktops
- Inputs
- Full duplex streams!!!



Output Screen
-------------

- Size
- Buffer?

Window
------

- Size/Bounding box
- Transform

Desktops
--------

- Size/Bounding box
- Transform

Inputs
------

- Input streams
- Control output stream


Streams
-------

- Pair of kernel streams?


It seems that screens, desktops and windows could share a common structure.

GraphicsObject
- Bounding box (2x vec2)
- Transform (matrix3x3)
- Id (uint32)
- Type (uint32)

JSON or BSON for the data protocol? Probably BSON for the differentiation between ints and floats.

Types
-----

- Screen, Desktop, Window, 


Protocol
--------

Binary format:

Header: (8 bytes total)
- Magic number Ls (uint16)
- Major Version (uint8)
- Minor Version (uint8)
- Total Size (uint32)

Types:
- Short String      0x01
- Lond String       0x02
- String Table      0x03
- String Reference  0x04    (basically an array)
- True              0x05
- False             0x06
- Uint              0x07
- Int               0x08
- Double            0x09
- Object            0x0A
- Array             0x0C
- ByteArray         0x0B

- Null              0x00
- String            0x01 0x11 (long)
- String Table      0x02 0x12 (long)
- String Ref        0x03 0x13 (long)
- Boolean           0x04 (false) 0x14 (true)
- Byte              0x05 (unsigned) 0x15 (signed)
- Short             0x06 (unsigned) 0x16 (signed)
- Int               0x07 (unsigned) 0x17 (signed)
- Float             0x08 0x18 (double)
- Array             0x09 0x19 (long)
- ByteArray         0x0A 0x1A (long)
- Object            0x0B 0x1B (long)
- SuperShort Str    0xXC supports string <= 16 long in the upper hex>
- Command Obj       0x0D 0x1D
- Total commands    0x0E

Example:
Ls\x01\x00          -- Magic/Version
\x02                -- String table
\x02                -- Number of elements in the array
\x01                -- String type
\x05                -- String length
Hello
\x01                -- Short String type
\x05                -- String length
Melli
\x0B                -- Object
\x02                -- Number of elements in the object
\x03                -- String ref
\x00                -- String ref index 0
\x03                -- String ref
\x01                -- String ref index 1

Connection with monitor
{'cmd':'connect_monitor','params':{'width':320,'height':240}}
Total 61 bytes

Testing
-------
Ls\x01\x00
\x02 \x05
\x01 \x03 cmd
\x01 \x0E connect_monitor
\x01 \x06 params
\x01 \x05 width
\x01 \x06 height
\x0B \x02
\x03 \x00
\x03 \x01
\x03 \x02
\x0B \x02
\x03 \x03
\x06 \x01\x40
\x03 \x04
\x05 \xF0
Total 69 bytes

Ls\x01\x00
\x0B \x02
\x01 \x03 cmd
\x01 \x0E connect_monitor
\x01 \x06 params
\x0B \x02
\x01 \x05 width
\x06 \x01\x40
\x01 \x06 height
\x05 \xF0
Total 57 bytes

Ls\x01\x00
\x0B \x02
\x3C cmd
\xEC connect_monitor
\x6C params
\x0B \x02
\x5C width
\x06 \x01\x40
\x6C height
\x05 \xF0
Total 52 bytes

Command examples
----------------


Commands:
- connect_monitor       0x01
- create_desktop        0x02
- create_window         0x03
- move_window           0x04
- move_element          0x05
- create_element        0x06

Create element types:
- Menu                  0x01
- Button                0x02
- Context menu          0x03
- Label                 0x04

Ls\x01\x00
\x0E \x01
\x0D            -- Command object
\x01            -- connect_monitor
\x02            -- Number of params, assumed to be an object
\x1C w
\x06 \x01\x40
\x1C h
\x05 \xF0
Total 18 bytes

Ls\x01\x00
\x0E \x02
\x0D
\x02            -- create_desktop
\x02            -- num params
\x1C i          -- desktop id
\x05 \x00
\x1C m          -- monitor id
\x05 \x00

\x0D
\x03            -- create_window
\x04            -- num params
\x1C d          -- desktop id
\x05 \x00
\x1C i          -- window id
\x05 \x00
\x1C w          -- width
\x06 \x01 \x40
\x1C h          -- height
\x05 \xF0
Total 37 bytes

\x0D
\x06            -- create_element
\x1C t          -- type
\x05 0x01

Test protocol Mk II
-------------------

Null            -- 0x00
Short Byte      -- 0xX1 (X = value)
Short UByte     -- 0xX2 (X = value)
Var int         -- 0xX3 (X = number of bytes)
Var uint        -- 0xX4 (X = number of bytes)
Short string    -- 0xX5 (length of string)
String          -- 0x06
Float           -- 0x07 0x17 (double)
Boolean         -- 0x08 (false) 0x18 (true)
Short Array     -- 0xX9 (X = num of elements)
Array           -- 0x0A
Short Object    -- 0xXB (X = num of elements)
Object          -- 0x0C
Short ByteArray -- 0xXD (X = num of elements)
ByteArray       -- 0x0E


L\x01
\x12            -- One command
\x01            -- connect_monitor
\x22            -- Two parameters
\x15 w          -- width
\x24 \x01\x40   -- 320 uint
\x15 h          -- height
\x14 \xF0       -- 240 uint
Total bytes 14


L\x01
\x22            -- Two commands
\x02            -- create_desktop
\x22            -- num params
\x15 d          -- desktop id
\x05
\x15 m          -- monitor id
\x05

\x03            -- create_window
\x42            -- num params
\x15 d          -- desktop id
\x05
\x15 i          -- window id
\x05
\x15 w          -- width
\x24 \x01\x40
\x15 h          -- height
\x14 \xF0
Total 28 bytes

Assume fixed

\x01            -- Version
\x22            -- Two commands
\x02            -- create_desktop
\x05            -- desktop id
\x05            -- monitor id
\x03            -- create_monitor
\x05            -- desktop id
\x05            -- window id
\x24 \x01\x40   -- width
\x14 \xF0       -- height
Total 13 bytes

Current classes
---------------

Monitor:        -- Represents a connected screen (represented in pixels)
Desktop:        -- Represents an area in virtual space (represented in floating point)
DisplayObject:  -- Top level graphical object, has a bounding box and position

DisplayObject types
- Label
- Button
- Text input
- Image
- Containers
- 3D context (OpenGL context?)

- Value object (label, button, checkbox, radio button, input box)


UIElements
- CSS classes
- Value source
- Parent

UI States
- Idle, hover, down
- Selected/not selected

Server:
- Defines layout
- Gives styles
- Updates layout

Client:
- Handles applying styles
- Handles animating styles
