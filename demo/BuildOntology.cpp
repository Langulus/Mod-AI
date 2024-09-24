///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Build ontology                                                          
///                                                                           
void BuildOntology(Thing& root) {
   root.LoadMod("AssetsGeometry");

   root.Run("##plural         = { ((number? > 1) or Range(1, +∞)) * ?? }");
   root.Run("##hi             = {thing? create user}");
   root.Run("##`,`            = {? conjunct!2 ??}");
   root.Run("##me             = {?.user}");
   root.Run("##my             = {##me.??}");
   root.Run("##I              = ##me");
   root.Run("##name           = name");
   root.Run("##is             = {? = ??}");
   root.Run("##let            = ##`,`");
   root.Run("##make           = {? create ??}");
   root.Run("##a              = ##(1)");
   //root.Run("##new            = "); // how do we define new??
   root.Run("##game           = (window, renderer, world)");
   root.Run("##called         = name(text??)");
   root.Run("##create         = ##make");
   root.Run("##place          = ##make");
   root.Run("##generate       = ##make");
   root.Run("##grid           = mesh(grid)");
   root.Run("##cube           = mesh(box)");
   root.Run("##cubes          = ##plural(##cube)");
   root.Run("##universe       = world");
   root.Run("##control        = {? create InputGatherer}");
   root.Run("##the            = {?.??}");
   root.Run("##camera         = camera");
   root.Run("##with           = ##make");
   root.Run("##mouse          = ("
      "Events::MouseMove, "
      "Events::MouseScroll, "
      "Keys::LeftMouse, "
      "Keys::MiddleMouse, "
      "Keys::RightMouse, "
      "Keys::Mouse4, "
      "Keys::Mouse5, "
      "Keys::Mouse6, "
      "Keys::Mouse7, "
      "Keys::Mouse8) "
   );

   root.Run("##WSAD           = (##W, ##S, ##A, ##D)");
   root.Run("##space          = Keys::Space");
   root.Run("##`left control` = Keys::LeftControl");
   root.Run("##and            = {?, ??}");
   root.Run("##giant          = ({?.organism.size * 4} or {##human.size * 4})");
   root.Run("##room           = ##invert(##cube)");
   root.Run("##solid          = solid(yes)");

   root.Run("##red            = Colors::Red");
   root.Run("##blue           = Colors::Blue");

   root.Run("##zero           = 0");
   root.Run("##one            = 1");
   root.Run("##two            = 2");
   root.Run("##three          = 3");
   root.Run("##four           = 4");
   root.Run("##five           = 5");
   root.Run("##six            = 6");
   root.Run("##seven          = 7");
   root.Run("##eight          = 8");
   root.Run("##nine           = 9");
   root.Run("##hundred        = {? * 100}");
   root.Run("##thousand       = {? * 1000}");
   root.Run("##million        = {? * 1000000}");

   root.Run("##`0`            = 0");
   root.Run("##`1`            = 1");
   root.Run("##`2`            = 2");
   root.Run("##`3`            = 3");
   root.Run("##`4`            = 4");
   root.Run("##`5`            = 5");
   root.Run("##`6`            = 6");
   root.Run("##`7`            = 7");
   root.Run("##`8`            = 8");
   root.Run("##`9`            = 9");

   root.Run("##of             = ");
   root.Run("##in             = Ranges::In");
   root.Run("##at             = Ranges::At");
   root.Run("##to             = Ranges::To");
   root.Run("##center         = Ranges::Center");
   root.Run("##behind         = Ranges::Behind");
   root.Run("##it             = ");
   root.Run("##randomly       = ");
   root.Run("##attach         = ");
   root.Run("##always         = ");
   root.Run("##run            = ");
   root.Run("##from           = ");
   root.Run("##if             = ");
   root.Run("##get            = ");
   root.Run("##gets           = ");
   root.Run("##`too close`    = ");
   root.Run("##them           = ");
   root.Run("##touch          = ");
   root.Run("##disappear      = ");
   root.Run("##all            = ");
   root.Run("##write          = ");
   root.Run("##`\"`           = ");
   root.Run("##stop           = ");
   root.Run("##player         = ");
   root.Run("##should         = ");
   root.Run("##be             = ");
   root.Run("##able           = ");
   root.Run("##move           = ");
   root.Run("##car            = ");
   root.Run("##cars           = ");
   root.Run("##using          = ");
   root.Run("##each           = ");
   root.Run("##second         = ");
   root.Run("##turns          = ");
   root.Run("##into           = ");
   root.Run("##bomb           = ");
   root.Run("##points         = ");
   root.Run("##hit            = ");
   root.Run("##hitting        = ");
   root.Run("##you            = ");
   root.Run("##lose           = ");
   root.Run("##win            = ");
   root.Run("##have           = ");
   root.Run("##single         = ");
   root.Run("##`'s`           = (##is or ##us or ##has)");
}