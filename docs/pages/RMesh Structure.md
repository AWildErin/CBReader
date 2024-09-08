# RMesh Structure
A Room Mesh (rmesh) file is the main room file in [SCP: Containment Breach](https://scpcbgame.com/). It is a basic mesh format designed to be house the room's mesh and baked entities.

The rmesh comprises of 5 main chunks of data:
- The header - Provides basic data about the mesh, such as whether or not it contains trigger boxes.
- The drawn mesh - The visual part of the mesh.
- The collision mesh - Not every room mesh has this, it's designed to add more collision for things like stairs or the HCZ tunnels.
- Trigger boxes - A list of trigger boxes which are used by the game code to handle certain events.
- Entities - Rooms have a specific list of entities that are baked in to the level, such as lights, models, sound emitters, screens and waypoints

## Structure

### Types
#### Vertex
A vertex is a struct that describes a mesh vertex.
The structure of a vertex is:
- X - float.
- Y - float.
- Z - float.
- UV X - float.
- UV Y - float.
- Lightmap UV X - float.
- Lightmap UV Y - float.
- R - byte
- G - byte
- B - byte

### Triangle
A triangle is a struct of 3 uint32's that define what vertices are apart of the specified triangle.

#### Surface
A surface is a struct that describes a collection of triangles and vertices of the mesh. Surfaces are broken up
by textures, meaning that all faces using the same texture are separated into 1 big surface.

A surface structure is as followed:
- Textures[2] - The first is usually the texture of the surface, and the second is usually the lightmap texture.
- Vertex Count - The count of all the vertices. Type is uint32.
- Vertices[Vertex Count] - An array of all the vertices for this surface.
- Triangle Count - The count of all the triangles. Type is uint32.
- Triangles[Triangle Count] - All the triangles for this surface.

### Texture
A texture is a basic struct containing a blend type and texture name.
The texture name is only present when the blend type does not equal zero (0).

NOTE: I'm not exactly sure what blend type is or what it's values are

### Header
The header is a size (uint32) prefixed string, usually it is either `RoomMesh` or `RoomMesh.HasTriggerBox`. (In SCP:CB NTF, they extended this to add `.HasNoColl`)

```cpp
std::uint32_t headerSize;
std::string headerText;
```

### Drawn Mesh
The drawn mesh is the visual mesh that is rendered with textures, but is also used for general collision geometry alongside the collision mesh.
It comprises of an array of surfaces, each of which describes a collection of vertices and triangles broken up by texture.

### Collision Mesh
The collision mesh is similar to the drawn mesh, except it is not drawn and is used purely for collision. Not every room mesh has collision mesh
surfaces, but notable rooms are 173's chamber for the stairs, and various HCZ rooms to stop the player from walking up the sides.

An important difference between the drawn mesh and the collision mesh, is that the surface vertices are only the vertex positions. They do not contain any
UV or color data.

### Trigger boxes
Trigger boxes are collision meshes with a name, they used by the same to handle starting certain events.
A room mesh only has trigger boxes if the header contains `.HasTriggerBox.`

Inside the rmesh, they are an array that starts with the size of the array, similar to how surfaces are defined.

The structure for a trigger boxes is:
- Collision Mesh - The actual mesh for the trigger box
- Name - A length prefixed string containing the name that is used by the game code.

### Entities
Various entities are baked into the room mesh file. 
They are defined in an array that starts with the entity count. The first value of an entity is always the entities class name (A length prefixed string),
and the class name is used to dictate how to read the entity's binary data as they are not in any specific order.

Below are the structure of various entities

#### Screen
A screen is a CB entity that displays a PNG on the player's view, usually hinting at lore for SCP:CB.

- Position X - float.
- Position Y - float.
- Position Z - float.
- Image path - A length prefixed string that contains the image path.

#### Waypoint
Waypoints are used the the in-game AI.

- Position X - float.
- Position Y - float.
- Position Z - float.

#### Light
Despite lightmaps being baked, light entities are also baked into the room and spawned in game.
If X, Y or Z is equal to zero, the light entity does not have range, color or intensity.

- Position X - float.
- Position Y - float.
- Position Z - float.
- Range - float. Range of the light. SCP:CB calculates this as `Range/2000.f` when creating the light in game.
- Color - Length prefixed string containing the RGB values. (Yes it's a string, I'm not sure why..)
- Intensity - float. Intensity value of the light. In-game, SCP:CB uses this as `Min(Intensity*0.8,1.0)`

#### Spotlight
Similar to lights, spotlights are also baked into the room mesh.
If X, Y or Z is equal to zero, the spotlight entity does not have range, color, intensity, angles, inner or outer cone angles.

- Position X - float.
- Position Y - float.
- Position Z - float.
- Range - float. Range of the light. SCP:CB calculates this as `Range/2000.f` when creating the light in game.
- Color - Length prefixed string containing the RGB values. (Yes it's a string, I'm not sure why..)
- Intensity - float. Intensity value of the light. In-game, SCP:CB uses this as `Min(Intensity*0.8,1.0)`
- Angle - Length prefixed string containing the pitch, yaw and roll. Only pitch and yaw are used in-game.
- Inner cone angle - uint32. Defines the inner cone angle for the spotlight
- Outer cone angle - uint32. Defines the outer cone angle for the spotlight

#### Sound Emitter
A sound emitter is an entity that plays a sound.
NOTE: This isn't complete as I don't fully understand the code surrounding the sound emitter entities.

- Position X - float.
- Position Y - float.
- Position Z - float.
- Sound - uint32.
- Range - float.

#### Player start
This entity is used to teleport the player when the room is loaded
- Position X - float.
- Position Y - float.
- Position Z - float.
- Angle - Length prefixed string containing the pitch, yaw and roll.

#### Model
A model entity that, when the room is loaded, spawns the mesh from the path and rotates, positions and scales it based on this entity's data.

- Path - A length prefixed string that has a path to the model this entity uses.
- Position X - float.
- Position Y - float.
- Position Z - float.
- Pitch - float.
- Yaw - float.
- Roll - float.
- Scale X - float.
- Scale Y - float.
- Scale Z - float.
