///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 19                                                                 
///                                                                           
/// Testing user interface creation and UI interpretation                     
///                                                                           
void Demo19(Thing& root) {
   auto ms = root.CreateChild("Demo19");
   //todo make with language
   /*ms->DoGASM("Create("
      "AWindow, CUISystem,"
      "CUIButton(`Button test`, Position(50, 100, 0)),"
      "CUIText(`Some static text`, Position(50, 200, 0)),"
      "CUIInput(`Some dynamic text, you should be able to write here! Since size is manually set, text should wrap!`, Position(50, 300, 0), Scale(300, 300, 1)),"
      "CUIFrame("
      "CUIText(`A vertical list`),"
      "CUIButton(`Button inside frame 1`),"
      "CUIButton(`Button inside frame 2`)"
      "),"
      "CUIFrame("
      "CUIText(`A horizontal list`),"
      "HorizontalLayout,"
      "CUIButton(`B1`),"
      "CUIButton(`B2`)"
      "),"*/


      /*
      "CUIFrame("
         "CUIButton(`Button inside list 1`), "
         "CUIText(`Text inside list`), "
         "CUIInput(`Editable text inside list`), "
         "CUIFrame("
            "CUIButton(`Button inside a list inside a list 1`), "
            "CUIText(`Text inside list inside a list`), "
            "CUIInput(`Editable text inside list inside a list`) "
         "), "
      ")"*/;
}