#version 450

struct globalStreams
{
    int sbase1_0;
};

layout(std140) uniform _globalCbuffer
{
    int Var1;
} _globalCbuffer_var;

globalStreams globalStreams_var;

int shaderA_Compute1()
{
    return _globalCbuffer_var.Var1 + globalStreams_var.sbase1_0;
}

int shaderA_Compute2()
{
    return shaderA_Compute1();
}

int main()
{
    return shaderA_Compute1() + shaderA_Compute2();
}

