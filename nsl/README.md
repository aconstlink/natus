
# nsl 

The language already has become quite complex and I can barely remember all the details. So lets dive into it again and give it a good documentation.

## Types
nsl supports various types:
```
float_t uint_t int_t vec1_t vec2_t vec3_t vec4_t mat2_t mat3_t mat4_t tex1d_t tex2d_t
```

## The Config

A configuration is supposed to hold properties, variables and shaders. A combination of the aforementioned is just like a config. So a configuration used like this:

```
config name 
{
  // render states (not yet)
  // variables
  // shaders
}
```

## The Shaders
A shader is defined within the ```config``` section. The following shaders are supported:
```
vertex_shader pixel_shader (more to come)
```
Shaders are defined like so:
```
config NAME
{
  vertex_shader NAME
  {
    // variables here
    
    void main()
    {
      // code here
    }
  }
  
  pixel_shader NAME
  {
    // variables here
    
    void main()
    {
      // code here
    }
  }
}
```
### Bindings
Input variables can be bound to some predefined name which is called a binding. nsl has the following bindings:
```
position normal tangent texcoord texcoord(0-7) color color(0-7) projection view world object camera camera_position viewport
```

### Ins and Outs
Ins and outs are used to define the data stream through the pipeline stages(i.e. shaders). An ```in``` is used to specify an incomping data stream. An ```out``` is used to specify data that is streaming out of a shader. If a shader is followed by another shader, the ```in```s in the shader that follows must match the ```out```s of the shader that came first.

```
vertex_shader NAME
{
  mat4_t proj : projection ;
  mat4_t view : view ;

  in vec3_t pos : position ;
  in vec2_t tx : texcoord0 ;
  
  out vec2_t tx : texcoord0 ;
  out vec4_t pos : position ;
  
  void main()
  {
    out.pos = vec4_t( in.pos, 1.0 ) ;
    out.tx = in.tx ;
  }
}

pixel_shader
{
    tex2d_t some_texture ;

    in vec2_t tx : texcoord0 ;
    out vec4_t color : color ;

    void main()
    {
        out.color = texture( some_texture, in.tx ) ;
    }
}
  
```


## Libraries
