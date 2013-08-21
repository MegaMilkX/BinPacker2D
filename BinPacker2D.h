#ifndef _BINPACKER2D_H_
#define _BINPACKER2D_H_

#include <vector>
#include <algorithm>

namespace bp2D
{

#define BINPACKPARAM_POWEROFTWO 0x1
#define BINPACKPARAM_ASS

#define SORT_LONGESTSIDE
#define SORT_SPACE
#define SORT_WIDTH
#define SORT_HEIGHT

struct BinRect
{
	unsigned int id;
	float x, y;
	float w, h;

	BinRect()
	{
		this->id = 0;
		this->x = 0;
		this->y = 0;
		this->w = 0;
		this->h = 0;
	}

	BinRect(int id, float w, float h)
	{
		this->id = id;
		this->x = 0;
		this->y = 0;
		this->w = w;
		this->h = h;
	}

	BinRect(int id, float x, float y, float w, float h)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	BinRect(float w, float h)
	{
		this->id = 0;
		this->x = 0;
		this->y = 0;
		this->w = w;
		this->h = h;
	}

	BinRect(float x, float y, float w, float h)
	{
		this->id = 0;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	float GetMaxSide()
	{
		if(w>h)
			return w;
		else
			return h;
	}
/*
	bool operator > (BinRect br)
	{return this->GetMaxSide() > br.GetMaxSide();}

	bool operator < (BinRect br)
	{return this->GetMaxSide() < br.GetMaxSide();}

	bool operator == (BinRect br)
	{return this->GetMaxSide() == br.GetMaxSide();}*/
};

bool BinRectMoreThan(BinRect r0, BinRect r1)
{
	return r0.GetMaxSide()>r1.GetMaxSide();
}

class BinNode
{
public:
	BinNode()
	{
		children[0] = 0;
		children[1] = 0;
		isFilled = false;
		parent = 0;
	}
	BinNode(BinNode* nodeToCopy)
	{
		children[0] = nodeToCopy->GetChildNode(0);
		children[1] = nodeToCopy->GetChildNode(1);
		isFilled = nodeToCopy->IsFilled();
		parent = nodeToCopy->GetParentNode();
		SetRect(nodeToCopy->GetRect());
	}
	~BinNode(){}

	BinNode* Insert(BinRect bin)
	{
		//Recursively find fitting leaf node for our bin
		BinNode* bn = this->_findEmptyLeafNode(bin);

		if(bn == 0)
		{
			_expand(bin);
			bn = this->_findEmptyLeafNode(bin);
		}

		if(bn)
		{
			printf("!!!Found fitting leaf node. Putting it in.\n");
			bn->Fit(bin);
			
		}
		else
		{
			printf("Couldn't fit even after _expand(). There's something wrong with your code.\n");
		}

		return bn;

		/*
		if(!IsLeaf())
		{
			//Not a leaf. Try to pass bin to children.
			if(children[0]->Insert(bin)==0)
				if(children[1]->Insert(bin)==0)
				{
					//Block doesn't fit in both child nodes
					if(!_expand(bin)) //Try to expand, this will fail if we are not the root node
					{
						return 0;
					}
				}
		}

		if(IsFilled())
		{
			if(parent==0)
			{
				_expand(bin);
			}
			else
				return 0;
		}
		//This node is a leaf and is empty

		return Fit(bin);*/
	}

	BinNode* Fit(BinRect bin)
	{	
		if(bin.w == rect.w && bin.h == rect.h)
		{
			//Fits perfectly
			this->rect.id = bin.id;
			this->isFilled = true;
			printf("\n");
			printf("Fit.\n");
			printf("-BinNode w=%f, h=%f\n", rect.w, rect.h);
			printf("-Bin w=%f, h=%f\n", bin.w, bin.h);
			printf("\n");
			return this;
		}
		else if(rect.w >= bin.w && rect.h >= bin.h)
		{
			printf("This size w=%f, h=%f\n", rect.w, rect.h);
			printf("Bin size w=%f, h=%f\n", bin.w, bin.h);

			//Volume is larger than passed rect
			if((rect.w-bin.w) > (rect.h-bin.h))
			{
				//_split(true, bin.h);
				_split(false, bin.w);
			}
			else
			{
				//_split(false, bin.w);
				_split(true, bin.h);
			}

			BinNode* bn = 0;
			if((bn = children[0]->Fit(bin))==0)
				return children[1]->Fit(bin);

			return bn;
		}
		else
		{
			printf("Doesn't fit.\n");
			return 0;
		}
	}

	BinNode* _findEmptyLeafNode(BinRect bin)
	{
		if(!IsLeaf())
		{
			//Not a leaf.
			BinNode* bn = 0;
			if((bn = children[0]->_findEmptyLeafNode(bin))==0)
				if((bn = children[1]->_findEmptyLeafNode(bin))==0)
					return 0;
			return bn;
		}

		if(IsFilled())
		{
			return 0;
		}
		
		if(rect.w>=bin.w && rect.h>=bin.h)
		{
			return this;
		}
		else
			return 0;
	}

	bool IsLeaf()
	{	return ((children[0] == 0) && (children[1] == 0));	}

	bool IsFilled()
	{
		if(IsLeaf())
			return isFilled;
		else
			return false;
	}

	BinNode* GetChildNode(int i)
	{	return children[i];	}
	BinNode* GetParentNode()
	{	return parent;	}
	BinRect GetRect()
	{	return rect;	}

	void FillVolumeVector(std::vector<BinRect> &vec)
	{
		if(IsLeaf())
		{
			if(IsFilled())
			{
				vec.insert(vec.end(), rect);
				return;
			}
			return;
		}
		else
		{
			children[0]->FillVolumeVector(vec);
			children[1]->FillVolumeVector(vec);
		}
	}

	void SetSize(float w, float h)
	{
		this->rect.w = w;
		this->rect.h = h;
	}

	void SetPos(float x, float y)
	{
		this->rect.x = x;
		this->rect.y = y;
	}

	void SetRect(BinRect r)
	{
		rect = r;
	}

	void SetParent(BinNode* p)
	{
		parent = p;
	}
private:
	BinNode* parent;
	BinNode* children[2];
	BinRect rect;
	bool isFilled;

	

	//Tries to split this node into two. But if passed bin fits perfectly, just returns this node. Otherwise returns first child node.
	void _split(bool horizontalSplit, float offset)
	{
		if(children[0]!=0 || children[1]!=0)
			return;

		children[0] = new BinNode();
		children[0]->SetParent(this);
		children[1] = new BinNode();
		children[1]->SetParent(this);

		if(horizontalSplit)
		{
			children[0]->SetRect(BinRect(rect.x, rect.y, rect.w, offset));
			children[1]->SetRect(BinRect(rect.x, rect.y+offset, rect.w, rect.h-offset));
			rect.h = rect.h + offset;
		}
		else
		{
			children[0]->SetRect(BinRect(rect.x, rect.y, offset, rect.h));
			children[1]->SetRect(BinRect(rect.x+offset, rect.y, rect.w-offset, rect.h));
			rect.w = rect.w + offset;
		}

		printf("Node has been splitten.\n");
	}	

	//Copy all node content to the first child, create second and fit rect inside
	bool _expand(BinRect bin)
	{
		if(parent == 0) //This is a root node
		{
			BinNode* newNode = new BinNode(this);
			children[0] = newNode;
			children[0]->SetParent(this);
			children[1] = new BinNode();
			children[1]->SetParent(this);
			this->isFilled = false;

			if(rect.w > rect.h)
			{
				children[1]->SetPos(0, rect.h);
				children[1]->SetSize(rect.w, bin.h);
				this->SetSize(rect.w, rect.h+bin.h);
			}
			else
			{
				children[1]->SetPos(rect.w, 0);
				children[1]->SetSize(bin.w, rect.h);
				this->SetSize(rect.w+bin.w, rect.h);
			}

			printf("-Child1 w=%f, h=%f\n", children[0]->GetRect().w, children[0]->GetRect().h);
			printf("-Bin w=%f, h=%f\n", bin.w, bin.h);
			printf("Expand is done.\n");
			return true;
		}
		else
			return false;
	}
};

class BinPacker2D
{
public:
	BinPacker2D()
	{
		rootNode = new BinNode();
	}
	~BinPacker2D()
	{
		volumes.clear();
		delete rootNode;
	}

	void AddRect(BinRect bin)
	{
		volumes.insert(volumes.end(), bin);
	}

	void Pack(int packingFlags)
	{
		if((packingFlags & BINPACKPARAM_POWEROFTWO) == BINPACKPARAM_POWEROFTWO){}
	
		//Sorting from largest to smallest rect (comparing by the largest side)
		std::sort(volumes.begin(), volumes.end(), BinRectMoreThan);

		//
		BinNode* n = 0;
		rootNode->SetSize(volumes[0].w, volumes[0].h);
		for(unsigned int i = 0; i < volumes.size(); i++)
		{
			n = rootNode->Insert(volumes[i]);
			volumes[i].x = n->GetRect().x;
			volumes[i].y = n->GetRect().y;
		}
	}

	std::vector<BinRect> GetVolumes()
	{
		return volumes;
	}

	BinRect GetRootRect()
	{
		return rootNode->GetRect();
	}
private:
	std::vector<BinRect> volumes;
	BinNode* rootNode;
};

}

#endif