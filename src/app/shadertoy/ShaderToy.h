#pragma once
#include <string>
#include <vector>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/cereal.hpp>
#include <cereal/cereal.hpp>

namespace shadertoy {

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
        std::string filter; //mipmap, nearest, linear
        std::string wrap;  // clamp, mirror, repeat
        std::string vflip; //bool
        std::string srgb;  //bool
        std::string internal;  // "byte" or "none"

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(filter), CEREAL_NVP(wrap), CEREAL_NVP(vflip), CEREAL_NVP(srgb), CEREAL_NVP(internal));
        }
    };

    struct Input {
        int id;
        std::string src;  //url or path
        std::string ctype; //buffer cubemap keyboard mic music musicstream texture video volume webcam
        int64_t channel; // channel number, [0,1,2,3]
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
        std::string code;  //fragmentshader
        std::string name;  // Image, Buf A, Buf B, Buf C Buf D, sometimes empty -.-
        std::string description;
        std::string type;  //image, buffer, sound, common

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

