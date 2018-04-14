#ifndef __QUICKTYPE_SHADERTOY_HPP__
#define __QUICKTYPE_SHADERTOY_HPP__

#include <string>
#include <vector>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/cereal.hpp>
#include <cereal/cereal.hpp>

namespace shadertoy {

//    enum class Ctype { BUFFER, CUBEMAP, KEYBOARD, TEXTURE };
//    enum class Filter { LINEAR, MIPMAP, NEAREST };
//    enum class Internal { BYTE };
//    enum class Srgb { FALSE, TRUE };
//    enum class Vflip { FALSE, TRUE };
//    enum class Wrap { CLAMP, REPEAT };
//    enum class Name { BUF_A, BUF_B, BUF_C, COMMON, EMPTY, IMAGE, SOUND };
//    enum class Type { BUFFER, COMMON, IMAGE, SOUND };
//
//    static const std::string Ctype_s[] = {"BUFFER","CUBEMAP","KEYBOARD","TEXTURE" };
//    static const std::string Filter_s[] = {"LINEAR","MIPMAP","NEAREST" };
//    static const std::string Internal_s[] = {"BYTE" };
//    static const std::string Srgb_s[] = {"FALSE", "TRUE" };
//    static const std::string Vflip_s[] = {"FALSE","TRUE" };
//    static const std::string Wrap_s[] = {"CLAMP","REPEAT" };
//    static const std::string Name_s[] = {"BUF_A","BUF_B","BUF_C","COMMON","EMPTY","IMAGE","SOUND" };
//    static const std::string Type_s[] = {"BUFFER","COMMON","IMAGE","SOUND" };
//
//    std::string Ctype_tostring( Ctype c )
//    {
//        return Ctype_s[static_cast<std::size_t >(c)];
//    }
//
//    Ctype Color_fromstring( std::string const & s )
//    {
//        auto lol = Ctype_s.find(s);
//        return ColorMapReverse[s];
//    }
//
//    namespace cereal
//    {
//        template <class Archive> inline
//        std::string save_minimal( Archive const &, Color const & t )
//        {
//            return Color_tostring( t );
//        }
//
//        template <class Archive> inline
//        void load_minimal( Archive const &, Color & t, std::string const & value )
//        {
//            t = Color_fromstring( value );
//        }
//    }

    static const char* image_header =
            "uniform vec3      iResolution;\n"
            "uniform float     iTime;\n"
            "uniform float     iChannelTime[4];\n"
            "uniform vec4      iMouse;\n"
            "uniform vec4      iDate;\n"
            "uniform float     iSampleRate;\n"
            "uniform vec3      iChannelResolution[4];\n"
            "uniform int       iFrame;\n"
            "uniform float     iTimeDelta;\n"
            "uniform float     iFrameRate;\n"
            "struct Channel\n"
            "{\n"
            "    vec3  resolution;\n"
            "    float time;\n"
            "};\n"
            "uniform Channel iChannel[4];\n"
            "uniform sampler2D iChannel0;\n"
            "uniform sampler2D iChannel1;\n"
            "uniform sampler2D iChannel2;\n"
            "uniform sampler2D iChannel3;\n"
            "void mainImage( out vec4 c,  in vec2 f );\n";

    static const char* image_footer =
            "\nout vec4 outColor;\n"
            "\nvoid main( void )"
            "{"
                "vec4 color = vec4(0.0,0.0,0.0,1.0);"
                "mainImage( color, gl_FragCoord.xy );"
                "color.w = 1.0;"
                "outColor = color;"
            "}";




    struct Info {
        std::string id;
        std::string date;
        int64_t viewed;
        std::string name;
        std::string username;
        std::string description;
        int64_t likes;
        int64_t published;
        int64_t flags;
        std::vector<std::string> tags;
        int64_t hasliked;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(id), CEREAL_NVP(date), CEREAL_NVP(viewed), CEREAL_NVP(name), CEREAL_NVP(username),
                    CEREAL_NVP(description), CEREAL_NVP(likes), CEREAL_NVP(published), CEREAL_NVP(flags),
                    CEREAL_NVP(tags), CEREAL_NVP(hasliked));
        }

    };
    struct Sampler {
        std::string filter;
        std::string wrap;
        std::string vflip;
        std::string srgb;
        std::string internal;
        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(filter), CEREAL_NVP(wrap), CEREAL_NVP(vflip), CEREAL_NVP(srgb), CEREAL_NVP(internal));
        }
    };

    struct Input {
        int64_t id;
        std::string src;
        std::string ctype;
        int64_t channel;
        struct Sampler sampler;
        int64_t published;
        template<class Archive>

        void serialize(Archive &archive) {
            archive(CEREAL_NVP(id), CEREAL_NVP(src), CEREAL_NVP(ctype), CEREAL_NVP(channel), CEREAL_NVP(sampler),
                    CEREAL_NVP(published));
        }
    };


    struct Output {
        int64_t id;
        int64_t channel;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(id, channel);
        }

    };

    struct Renderpass {
        std::vector<struct Input> inputs;
        std::vector<struct Output> outputs;
        std::string code;
        std::string name;
        std::string description;
        std::string type;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(inputs), CEREAL_NVP(outputs), CEREAL_NVP(code), CEREAL_NVP(name),
                    CEREAL_NVP(description), CEREAL_NVP(type));
        }
    };


    struct Shader {
        std::string ver;
        struct Info info;
        std::vector<struct Renderpass> renderpass;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(ver), CEREAL_NVP(info), CEREAL_NVP(renderpass));
        }
    };
}
#endif
