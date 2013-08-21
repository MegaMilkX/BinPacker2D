BinPacker2D
===========
'''c++
bp2D::BinPacker2D *binPacker = new bp2D::BinPacker2D();

binPacker->AddRect(bp2D::BinRect(800, 800));

binPacker->Pack(BINPACKPARAM_POWEROFTWO, SORT_MAXSIDE);

std::vector<bp2D::BinRect> rects = binPacker->GetVolumes();
bp2D::BinRect rootRect = binPacker->GetRootRect();

delete binPacker;
'''