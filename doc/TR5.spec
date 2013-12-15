/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
#############################################################################
#
#  TR5 File Format?
#  rgbold, Turbo Pascal, and Mongoose spec merge
#
#  NOTES: 
#  I use ISO style types to be more clear, not word sizes
#       
#     Int   :  unsigned short  : bitu16
#     Dword :  unsigned int    : bitu32
#
#  Some other markers?  I think these may be:
#  
#  58 50 50 FF    "XPP."   |  "PPX"
#  58 50 48 FF    "XPH."   |  "HPX"
#  50 50 48 FF    "PPH."   |  "PPH"
#
#  They appear in a patern like:
#  ALEX
#     XPP XPP ...
#  ALEX
#  
#
#  - Mongoose
#
# Room Vertices are 28 bytes each
# 3 floats representing the x,y,z values of vertex
# 3 floats probably the vertex normals
# 1 DWORD  probably diffuse color for the vertex
# 
# Rectangles are same old format of V0,V1,V2,V3,Texture, followed by
# Int unknown, usually zero.
# Texture needs to be masked with 0xfff (high 4 bits are flags)
# 
# Triangles are same old format of V0,V1,V2,Texture, followed by
# Int unknown, usually zero.
# Texture needs to be masked with 0xfff (high 4 bits are flags)
# 
# TR5 uses D3D extensively. The light format is a prime example. The last
# byte in the light field before the CDCDCD is the light type which in
# D3d is 1,2, or 3 for point, spot, or directional lights respectively.
# Also somewhere in the unknowns must be the fog values.
# 
# Sound Map is now 900 bytes
# 
# Before SoundSources is a new field for Flyby_Camera data
# bitu32 Number_of_FBCameras
# followed by data for each camera. Data field is 40 bytes.
# [Andy had previously stated NumXYZ as a new field incorrectly.
# His NumXYZ field was the SoundSources in normal structure format]
# 
# 
# Cinematic Frames now 24 bytes
# 
# There are no colored rectangles or triangles in mesh data.
# Mesh Rectangles and Triangles now same format as room rects and tris,
# MeshRects: v0,v1,v2,v3,texture,int unknown
# MeshTris:  v0,v1,v2,texture,int unknown
# Texture Objects now preceeded by TEX/0 (4bytes)
# Object Textures now 38 bytes vice 20
# Tile is now a bitu32 vice bitu16 seems need to mask with 0xff as other
#        bits are flags. Seems bit15 set means triangle.
# In the texture coords now 0 is used for low value vice 1
# The extra 16 bytes at the end of ObjectTexture structure are 4 Dword
#        coords, UV?.
# Attributes using 1 or 2 for transparent. 2 used for 2 sided textures
#                (water surface,etc)
# 
# 
# Animation now 40 vice 32 bytes. extra 8 seem to be zero right before
# /after the old 8 unknowns
# 
# "SPR"  3 byte ascii before sprite textures
# 
# As TR4 there are no 8bit or 16 bit palettes (no colored rects or tris)
# No lightmap
# No 8bit textures
# 3 blocks of zlib compressed textures (32bit,16bit,sprite/misc)before
# level data.
# Level data not compressed.
# 
# Following level data are Riff Wave Formats.
# 
# Lara 2 meshes as TR4, skin and joints.
# 
# also , I was just reading thru the rectanges and triangles and
# thought the vertices always followed right after but appears
# that sometimes there is a 'pad' int of 0xCDCD.
# 
# 
# Also as Michiel has just posted the Object Textures for TR5 are
# 40bytes, sorry about that.
#  - Roy
#
#############################################################################


uint32 version;                   //  Will be "TR4\0" for valid header

// Textures /////////////////
bitu16 num_room_textures
bitu16 number of num_other_textures
bitu16 number of num_bump_map_textures
bitu32 UncompressedSize1
bitu32 CompressedSize1
TextureBlock1
bitu32 UncompressedSize2
bitu32 CompressedSize2
TextureBlock2
bitu32 UncompressedSize3
bitu32 CompressedSize3
TextureBlock3

/// Follows after textures


32 bytes unknown //mostly zero
bitu32 SizeData    // size of file to Riffs
bitu32 SizeData    // repeat of above
bitu32 Unknown     // seems always zero
bitu32 NumberRooms // number of rooms this level

{ Following Repeats for each Room in level

bitu32 "XELA" (someone named Alex or "dirX Enhanced Level Archive"
bitu32 size //size to next XELA block
bitu32 CDCDCDCD

Block1 60 bytes
{

bitu32 ublock1 //mostly: start of "XELA" +216 + ublock1 = FD end,
                but have seen 0xffffffff. Better using next data
                and compute FD size the old way of X*Z*8
bitu32 ublock2 //  start of "XELA" + 216 + ublock2 = FD start
bitu32 ublock3 // unknown
bitu32 ublock4 // possibly start of "XELA" + 216 + ublock4 = end portals

bitu32 Room X      // room's x position in world coords
bitu32 unknown1    // always seems to be 0000
bitu32 Room Z      // room's z position in world coords
bitu32 Ybottom     // room's y bottom positon in world coords
bitu32 Ytop        // room's y top positon in world coords

Int  NumZSectors // number of z sectors
Int  NumXSectors // number of x sectors

tr2_color4        // 4 bytes color info rgba

Int  NumLights
Int  NumStaticMeshes

bitu32 Unknown2    // seems always 0x0001
bitu32 Unknown3    // seems always 0x7fff
bitu32 Unknown4    // seems always 0x7fff

}close 60 byte block

bitu32 CDCDCDCD
bitu32 CDCDCDCD

20 byte block
{
6 bytes 0xff
Int Room_Flag    //room attributes (rain,etc)
Int unknown5
10 bytes 0x00
} close 20 byte block

bitu32 CDCDCDCD

16 byte block
{
unknown 16 bytes
}close 16 byte block

bitu32 CDCDCDCD
bitu32 CDCDCDCD
bitu32 CDCDCDCD
bitu32 CDCDCDCD

bitu32 Unknown6

bitu32 CDCDCDCD

56 byte block
{
bitu32 NumberTriangles
bitu32 NumberRectangles
bitu32 Unknown7 // always 0x0000 ??
bitu32 LightSize // number lights times 88 bytes
bitu32 NumberLights
bitu32 NumUnknown_36_byte_struct
bitu32 Unknown9
bitu32 Unknown10
bitu32 NumberLayers //TR5 'layers' room pieces
bitu32 Unknown11    // start of "XELA" + 216 + Unknown11 = start of 
layer 
info
bitu32 Unknown12    // start of "XELA" + 216 + Unknown12 = start of 
vertices
bitu32 Unknown13    // start of "XELA" + 216 + Unknown13 = start of 
rect/tri
bitu32 Unknown14    // same as Unknown13???
bitu32 Unknown15    // start of vertices + Unknown15 = start of next 
"XELA"
}close 56 byte block

bitu32 CDCDCDCD
bitu32 CDCDCDCD
bitu32 CDCDCDCD
bitu32 CDCDCDCD

// Lights //////////////////////////
if lights then 85 bytes light data followed by CDCDCD for each light

// Unknown /////////////////////////
if NumUnknown_36_byte_struct then read them too

// Sectors /////////////////////////
Sector Data (sectors times 8 bytes) normal sector data structure

// Portals /////////////////////////
Int NumberDoors

NumberDoors times 32 bytes (normal portal structure)

Int CDCD

// Static meshes //////////////////
if StaticMeshes than numberofstatics times 20 bytes each (normal 
structure)


// Layers ////////////////////////
Layer Data blocks. 56 bytes each.
{
   bitu32 NumberVertices
   Int  Unknown16
   Int  NumberRectangles
   Int  NumberTriangles

   unknown 46 bytes // was 48 in older docs

}close Layer Data block

following the Layer Data block are:
  rectangles for Layer1 // see format under NOTES
  triangles  for Layer1 // see format under NOTES
  rectangles for Layer2
  triangles  for Layer2

  ...and so on untill all layers are done

after the rectangle and triangle information are the room vertices
which are described under NOTES


}Close Rooms Loop



