
# nsl 

The language already has become quite complex and I can barely remember all the details. So lets dive into it again and give it a good documentation.

nsl helps the user to write shaders across platforms. The nsl shader is parsed, checked for simple syntax issues and is then transformed into the particular backend supported hardware shader. 

## Variable Types
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
## MRT
If the user wants to output onto multiple render targets:
```
pixel_shader
{
    // some vars
    
    out vec4_t color0 : color0 ;
    out vec4_t color1 : color1 ;
    out vec4_t color2 : color2 ;

    void main()
    {
        out.color0 = vec4_t(1.0) ;
        out.color1 = vec4_t(0.0) ;
        out.color2 = texture(...) ;
    }
}
```
The render targets can be specified via the C++ program using a ```framebuffer```.

## Libraries
A library can be used to reuse code. A library is written like this:
```
library mylib
{
    vec4_t my_color2( vec4_t color ) 
    {
        return color * vec4_t( 0.0, 1.0, 0.0, 0.0) ;
    }

    vec4_t my_color( vec4_t color )
    {
        return color + vec4_t( 1.0,0.0,0.0,0.0);
    }
    
    library inner
    {
      float_t pi = 3.1432423 ;
      
      vec2_t_t funk( in vec2_t vin ) {            
            return vin ;
        }
    }
    
    vec3_t xyz( vec3_t vin )
    {
        return vin ;
    }
    
    vec4_t to_vec4( in vec3_t vin ) {
        return vec4_t( nsl.mylib.xyz( vin ), 1.0 ) ;
    }
}
```

The simbols must be introduced to the used nsl database before those can be used within shaders. Using a library need to be preceeded by the word ```nsl``` and can be used within libraries too:

```
vertex_shader
{
    // some variables here
    void main()
    {
        vec3_t pos = nsl.mylib.xyz( nsl.mylib.xyz( in.pos * vec3_t( nsl.mylib.inner.pi) ) ) ;
        out.pos = proj * view * nsl.math.to_vec4( pos ) ;
    }
}
```
Within a library, you can define another library which help organizing code.

## Control Flow

```
if( condition ){}
while( condition ){}
for( int i=0; i<NUMBER; ++i ){ /*use i here*/ }
```

## Comments
Are written like so:
```
// one line comment
/* multi line comment*/
```

## Build-Ins
Build-in functions are provided with nsl as follows:
```
dot cross pulse step mix pow texture
```
