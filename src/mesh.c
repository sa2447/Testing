#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf3d_swapchain.h"
#include "gf3d_buffers.h"
#include "gf3d_obj_load.h"
#include "gfc_list.h"

#include "gf3d_camera.h"
#include "gf3d_mesh.h"

#define MESH_ATTRIBUTE_COUNT 3

typedef struct
{
    Mesh* mesh_list;

    Uint32 mesh_count;
    Uint32 chain_length;

    VkDevice device;

    Pipeline* pipe;


    VkVertexInputAttributeDescription attributeDescriptions[MESH_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription bindingDescription;


    Texture* defaultTexture;



}MeshManager;


extern int __DEBUG;
static MeshManager mesh_manager = { 0 };

void gf3d_mesh_manager_close();


void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;
    if (mesh_max == 0)
    {
        slog("cannot intilizat sprite manager for 0 sprites");
        return;
    }
    else
    {
        slog("we have mesh_max");
    }
    
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    mesh_manager.mesh_list = (Mesh *)gfc_allocate_array(sizeof(Mesh), mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

    
    gf3d_mesh_get_attribute_descriptions(&count);
    

    VkDevice device;
    const char* configFile;
    VkExtent2D extent;
    Uint32 descriptorCount;
    const VkVertexInputBindingDescription* vertexInputDescription;
    const VkVertexInputAttributeDescription* vertextInputAttributeDescriptions;
    Uint32 vertexAttributeCount;
    VkDeviceSize bufferSize;
    VkIndexType indexType;

    device = gf3d_vgraphics_get_default_logical_device();
    if (!device)
    {
        slog("no device");
        return;
    }

    configFile = "config/model_pipeline.cfg";
    if (!configFile)
    {
        slog("no config");
        return;
    }

    extent = gf3d_vgraphics_get_view_extent();
    if (&extent == NULL)
    {
        slog("no extent");
        return;
    }

    if (!count)
    {
        slog("no count");
        return;
    }

    if (!mesh_manager.attributeDescriptions)
    {
        slog("no ads");
        return;
    }


    int check = sizeof(MeshUBO);

    if (!check)
    {
        slog("no size of check");
        
        return;
    }
    else
    {
        slog("something");
    }

    indexType = VK_INDEX_TYPE_UINT16;
    if (!indexType)
    {
        slog("no index type");
        return;
    }
    
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_manager_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );

    

    
    mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");
    if (__DEBUG)slog("mesh manager initiliazed");
    atexit(gf3d_mesh_manager_close);
}

Mesh* gf3d_mesh_new()
{
    int i;
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        if (mesh_manager.mesh_list[i]._refCount > 0)continue;
        memset(&mesh_manager.mesh_list[i], 0, sizeof(Mesh));
        mesh_manager.mesh_list[i]._refCount = 1;
        return &mesh_manager.mesh_list[i];
    }
    return NULL;

}

Mesh* gf3d_mesh_load_obj(const char *filename)
{
    slog("mesh load obj called");
    int i;
    if (!filename)
    {
        slog("filename does not exist to load");
        return NULL;
    }
          
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        if (!mesh_manager.mesh_list[i]._inuse)continue;
        if (gfc_stricmp(mesh_manager.mesh_list[i].filename, filename) == 0)
        {
            return &mesh_manager.mesh_list[i];
        }
    }
    return NULL;

}

Mesh* gf3d_mesh_copy(Mesh* in)
{

}

void gf3d_mesh_move_vertices(Mesh* in, GFC_Vector3D offset, GFC_Vector3D rotation)
{

}

MeshPrimitive* gf3d_mesh_primitive_new()
{
    return gfc_allocate_array(sizeof(MeshPrimitive), 1);
}

VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{

    slog("called for ads");

    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);

    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 2;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count) *count = MESH_ATTRIBUTE_COUNT;


    return mesh_manager.attributeDescriptions;
}

VkVertexInputBindingDescription * gf3d_mesh_manager_get_bind_description()
{
    slog("called for binds");
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}


void gf3d_mesh_manager_close()
{
    int i;
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        if (mesh_manager.mesh_list[i]._refCount > 0) {
            gf3d_mesh_free(&mesh_manager.mesh_list[i]);
        }
    }
    if (mesh_manager.mesh_list)
    {
        free(mesh_manager.mesh_list);
    }
    gf3d_pipeline_free(mesh_manager.pipe);
    gf3d_texture_free(mesh_manager.defaultTexture);
    memset(&mesh_manager, 0, sizeof(MeshManager));
}


void gf3d_mesh_free(Mesh* mesh)
{

}

void gf3d_mesh_reset_pipes()
{

}

void gf3d_mesh_submit_pipe_commands()
{

}

VkCommandBuffer gf3d_mesh_get_model_command_buffer()
{

}

void gf3d_mesh_queue_render(Mesh *mesh, Pipeline *pipe, void *uboData, Texture *texture)
{
    int i, c;
    MeshPrimitive* prim;
    if (!mesh) return;
    if (!pipe) return;
    if (!uboData) return;
    c = gfc_list_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;
        gf3d_mesh_primitive_queue_render(prim, pipe, uboData, texture);
    }
}

void gf3d_mesh_render(Mesh* mesh, VkCommandBuffer commandBuffer, VkDescriptorSet* descriptorSet)
{

}

void gf3d_mesh_render_generic(Mesh* mesh, Pipeline* pipe, VkDescriptorSet* descriptorSet)
{

}

void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive* primitive)
{

}

Pipeline* gf3d_mesh_get_pipeline()
{
    return mesh_manager.pipe;
}

MeshUBO gf3d_mesh_get_ubo(GFC_Matrix4 modelMat,GFC_Color colorMod)
{
    MeshUBO ubo = { 0 };
    gfc_matrix4_copy(ubo.model, modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);
    ubo.color = gfc_color_to_vector4(colorMod);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1);
    return ubo;

}

void gf3d_mesh_draw(Mesh *mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture *texture, GFC_Vector3D lightPos, GFC_Color lightColor)
{
    MeshUBO ubo = { 0 };

    if (!mesh)return;
    gfc_matrix4_copy(ubo.model, modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);

    ubo.color = gfc_color_to_vector4f(mod);
    ubo.lightColor = gfc_color_to_vector4f(lightColor);
    ubo.lightPos = gfc_vector3dw(lightPos, 1.0);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1.0);
    gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
}


void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim, Pipeline *pipe, void *uboData, Texture *texture)
{
    if (!prim) return;
    if (!pipe) return;
    if (!uboData) return;

    if (!texture)texture = mesh_manager.defaultTexture;
    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer,
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
}


void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vertex *vertices;
    Uint32 vcount;
    size_t bufferSize;
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    if (!prim)
    {
        slog("no mesh primitive provided");
        return;
    }

    vertices = prim->objData->faceVertices;
    vcount = prim->objData->face_vert_count;
    bufferSize = sizeof(Vertex) * vcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->vertexBuffer, &prim->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    prim->vertexCount = vcount;

}

void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    Face *faces = NULL;
    Uint32 fcount;

    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkDeviceSize bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if (!prim) return;
    if (!prim->objData) return;
    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;
    if (!faces) return;
    if (!fcount) return;
    bufferSize = sizeof(Face) * fcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    prim->faceCount = fcount;
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

Mesh* mesh_load(const char* filename)
{

    slog("called mesh load");

    Mesh* mesh;
    ObjData* data;
    MeshPrimitive* prim;
    if (!filename) return NULL;

    mesh = gf3d_mesh_load_obj(filename);
    if (mesh) {
        mesh->_refCount++;
        return mesh;
    }


    mesh = gf3d_mesh_new();
    if (!mesh)
    {
        slog("no mesh");
        return NULL;
    }
    mesh->primitives = gfc_list_new_size(1);
    data = gf3d_obj_load_from_file(filename);
    if (!data) {
        slog("no data");
        return NULL;
    }
    prim = gf3d_mesh_primitive_new();
    if (!prim) {
        slog("no prim");
        return NULL;
    }
    gfc_list_append(mesh->primitives, prim);
    prim->objData = data;

    gf3d_mesh_primitive_create_vertex_buffers(prim);
    gf3d_mesh_setup_face_buffers(prim);
    gfc_line_cpy(mesh->filename, filename);
    return mesh;
}