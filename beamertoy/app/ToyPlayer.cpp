//
// Created by bone on 06.08.18.
//

#include "ToyPlayer.h"

namespace minuseins {

    struct Renderpass {

    };

    ToyPlayer::ToyPlayer() {

    }

    void ToyPlayer::init() {
      for (auto &pass : params_.renderpass) {
        if ("image" == pass.type) {
          image = pass;
        } else if ("buffer" == pass.type) {
          buffers.push_back(pass);
        } else if ("common" == pass.type) {
          common = pass;
        } else {
          std::cout << pass.type << "is unsupported, sorry" << std::endl;
        }
      }

      bool hasCommonPass = !common.code.empty();

    }
}
