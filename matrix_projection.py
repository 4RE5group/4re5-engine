import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Step 1: Define a 3D cube (8 vertices, 6 faces)
vertices = np.array([
    [-2, -1, -2],  # 0
    [1, -1, -1],   # 1
    [1, 1, -1],    # 2
    [-1, 1, -1],   # 3
    [-1, -1, 1],   # 4
    [1, -1, 1],    # 5
    [1, 1, 1],     # 6
    [-1, 1, 1]     # 7
])

# Define the 6 faces of the cube (each face is a list of vertex indices)
faces = [
    [0, 1, 2, 3],  # Bottom
    [4, 5, 6, 7],  # Top
    [0, 1, 5, 4],  # Front
    [2, 3, 7, 6],  # Back
    [0, 3, 7, 4],  # Left
    [1, 2, 6, 5]   # Right
]

# Step 2: Camera and projection parameters
# Camera position and orientation
camera_pos = np.array([3, 2, 5])
look_at = np.array([0, 0, 0])
up = np.array([0, 1, 0])

# Field of view and screen dimensions
fov = np.pi / 3  # 60 degrees
aspect_ratio = 1
near = 0.1
far = 100
screen_width, screen_height = 800, 600

# Step 3: View matrix
# Camera basis vectors
z_axis = (camera_pos - look_at) / np.linalg.norm(camera_pos - look_at)
x_axis = np.cross(up, z_axis) / np.linalg.norm(np.cross(up, z_axis))
y_axis = np.cross(z_axis, x_axis)

# View matrix
view_matrix = np.array([
    [x_axis[0], x_axis[1], x_axis[2], -np.dot(x_axis, camera_pos)],
    [y_axis[0], y_axis[1], y_axis[2], -np.dot(y_axis, camera_pos)],
    [z_axis[0], z_axis[1], z_axis[2], -np.dot(z_axis, camera_pos)],
    [0, 0, 0, 1]
])

# Step 4: Projection matrix
f = 1 / np.tan(fov / 2)
projection_matrix = np.array([
    [f / aspect_ratio, 0, 0, 0],
    [0, f, 0, 0],
    [0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)],
    [0, 0, -1, 0]
])

# Step 5: Transform vertices to screen coordinates
def transform_to_screen(vertices, view_matrix, projection_matrix, screen_width, screen_height):
    # Add homogeneous coordinate
    vertices_homogeneous = np.hstack([vertices, np.ones((vertices.shape[0], 1))])
    
    # Apply view and projection
    vertices_view = (view_matrix @ vertices_homogeneous.T).T
    vertices_clip = (projection_matrix @ vertices_view.T).T
    
    # Perspective divide
    vertices_ndc = vertices_clip[:, :3] / vertices_clip[:, 3:4]
    
    # Map to screen coordinates
    vertices_screen = np.zeros((vertices_ndc.shape[0], 2))
    vertices_screen[:, 0] = (vertices_ndc[:, 0] + 1) * 0.5 * screen_width
    vertices_screen[:, 1] = (1 - vertices_ndc[:, 1]) * 0.5 * screen_height
    
    return vertices_screen

# Transform vertices
vertices_screen = transform_to_screen(vertices, view_matrix, projection_matrix, screen_width, screen_height)

# Step 6: Draw the result
fig, ax = plt.subplots(figsize=(10, 8))

# Draw each face
for face in faces:
    face_vertices = vertices_screen[face]
    ax.fill(face_vertices[:, 0], face_vertices[:, 1], alpha=0.5)
    ax.plot(np.append(face_vertices[:, 0], face_vertices[0, 0]), 
            np.append(face_vertices[:, 1], face_vertices[0, 1]), 'k-')

# Set plot limits and labels
ax.set_xlim(0, screen_width)
ax.set_ylim(0, screen_height)
ax.set_title('3D Cube Projected to 2D Screen')
ax.set_aspect('equal')
ax.invert_yaxis()

plt.show()