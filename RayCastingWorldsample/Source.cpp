#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_RAYCASTWORLD
#include "olcPGEX_RayCastWorld.h"
#include <string>



// ADAPTOR CLASS - Override the RayCastWorld Engine and fill in the blanks!
class ExampleGame : public olc::rcw::Engine
{
public:
	ExampleGame(const int screen_w, const int screen_h, const float fov)
		: olc::rcw::Engine(screen_w, screen_h, fov)
	{
		sMap =
			"################################################################"
			"#.........#....................##..............................#"
			"#.........#....................................................#"
			"#.........#....................................................#"
			"#.........#....................................................#"
			"#.........#############........................................#"
			"#...............#..............................................#"
			"#...............#..............................................#"
			"#...............#..............................................#"
			"#.....o..#..#...#..............................................#"
			"#...............#..............................................#"
			"#...............#..............................................#"
			"#.....o..#..#..................................................#"
			"#..............................................................#"
			"#..............................................................#"
			"#..............................................................#"
			"#..............................................................#"
			"#.....................######..#................................#"
			"#.....................#.......#................................#"
			"#....................##.###.###.........................#......#"
			"#....................##.....#........................##........#"
			"#....................##.#####........................##.#......#"
			"#....................#.#.......................................#"
			"#....................#..#...............................#......#"
			"#..............................................................#"
			"#..............................................................#"
			"#..............................................................#"
			"#..............................................................#"
			"#..............................##..............................#"
			"#..............................##..............................#"
			"#..............................##..............................#"
			"################################################################";

		vWorldSize = { 64, 32 };
		gfxWall.Load("stonewall.png");
		gfxFloor.Load("sand.png");
		gfxbuilding.Load("wood.png");
		saber.Load("pull.png");
		star.Load("sun.png");
		r2d2.Load("droid.png");
		reticle.Load("reticle1.png");
		reticleGlow.Load("reticle2.png");

	}

protected:
	// User implementation to retrieve appropriate graphics for scenery
	olc::Pixel SelectSceneryPixel(const int tile_x, const int tile_y, const olc::rcw::Engine::CellSide side, const float sample_x, const float sample_y, const float distance) override
	{
		olc::Pixel p;

		// Choose appropriate colour
		switch (side)
		{
		case olc::rcw::Engine::CellSide::Top: // Location is "Sky"
			p = olc::CYAN;
			break;

		case olc::rcw::Engine::CellSide::Bottom: // Location is "Ground"

			p = gfxFloor.Sprite()->Sample(sample_x, sample_y);
			//p = olc::DARK_GREEN;
			break;

		default: // Location is "Wall"
			//p = olc::WHITE;
			//if (sample_x < 0.05f || sample_x > 0.95f || sample_y < 0.05f || sample_y > 0.95f)
			//{
			//	p = olc::BLACK;
			//}
			if (sMap[int(tile_y) * vWorldSize.x + int(tile_x)] != '#')
			{
				sMap[int(tile_y) * vWorldSize.x + int(tile_x)] = 'o';
				//p = gfxWall.Sprite()->Sample(sample_x, sample_y);
			}
			else {
				p = gfxWall.Sprite()->Sample(sample_x, sample_y);
			}
			if (sMap[int(tile_y) * vWorldSize.x + int(tile_x)] == 'o')
			{
				p = gfxbuilding.Sprite()->Sample(sample_x, sample_y);
			}
			
			break;
		}
		

		float fDistance = 1.0f - std::min(distance / 32.0f, 1.0f);

		float fShadow = 1.0f;
		switch (side)
		{
		case olc::rcw::Engine::CellSide::South: fShadow = 0.3f; break;
		case olc::rcw::Engine::CellSide::East: fShadow = 0.3f; break;
		}

		olc::vf2d vStarPos = mapObjects[1]->pos;
		olc::vf2d vPixelPos = olc::vf2d(float(tile_x) + sample_x, float(tile_y) + sample_y);
		float fStarDistance = (vStarPos - vPixelPos).mag2();
		float fStarLight = std::max(0.2f, 1.0f - std::min(fStarDistance / 10.0f, 1.0f));

		if (side == olc::rcw::Engine::CellSide::Top)
		{
			fStarLight = 0.2f;
		}

		fStarLight = 1.0f;

		p.r = uint8_t(float(p.r) * fDistance * fStarLight);
		p.g = uint8_t(float(p.g) * fDistance * fStarLight);
		p.b = uint8_t(float(p.b) * fDistance * fStarLight);

		

		// Apply directional lighting, by first creating a shadow scalar...

		return p;
	}

	// User implementation to retrieve if a particular tile is solid
	bool IsLocationSolid(const float tile_x, const float tile_y) override
	{
		if (int(tile_x) >= 0 && int(tile_x) < vWorldSize.x && int(tile_y) >= 0 && int(tile_y) < vWorldSize.y)
		{
			return sMap[int(tile_y) * vWorldSize.x + int(tile_x)] != '.';


		}


		return true;
	}


	// NOTE! Objects are not used in this demonstration ===================

	// User implementation to retrieve dimensions of an in game object
	float GetObjectWidth(const uint32_t id) override
	{
		if (id == 2)
			return 0.5f;
		return 1;
	}

	float GetObjectHeight(const uint32_t id) override
	{
		if (id == 2)
			return 0.5f;
		return 1;
	}

	// User implementation to retrieve appropriate graphics for objects
	olc::Pixel SelectObjectPixel(const uint32_t id, const float sample_x, const float sample_y, const float distance, const float angle) override
	{
		olc::Pixel p = olc::MAGENTA;

		if (id == 1)
		{
		p = star.Sprite()->Sample(sample_x, sample_y);
		}
		if (id == 2)
		{
			olc::vf2d voffset;
			if (angle >= 3.14159f * 0.0f && angle < 3.14159f * 0.5f) voffset = { 0.0f, 0.1f };
			if (angle >= 3.14159f * 0.5f && angle < 3.14159f * 1.0f) voffset = { 0.0f, 1.1f };
			if (angle >= 3.14159f * 1.0f && angle < 3.14159f * 1.5f) voffset = { 0.0f, 2.3f };
			if (angle >= 3.14159f * 1.5f && angle < 3.14159f * 2.0f) voffset = { 0.0f, 3.4f };


			olc::vf2d vSample = ((voffset + olc::vf2d(sample_x, sample_y)) *44.0f) / olc::vf2d(float(r2d2.Sprite()->width), float(r2d2.Sprite()->height));

			//p = r2d2.Sprite()->Sample(vSample.x, vSample.y);
		}
		
		if (id == 3)
		{
			p = reticle.Sprite()->Sample(sample_x, sample_y);
		}


		return p;
	}

	void HandleObjectVsScenery(std::shared_ptr<olc::rcw::Object> object, const int tile_x, const int tile_y, const olc::rcw::Engine::CellSide side, const float offset_x, const float offset_y) override
	{
		switch (side)
		{
		case olc::rcw::Engine::CellSide::North:
		case olc::rcw::Engine::CellSide::South:
			object->vel.y *= -1.0f;
			break;
		case olc::rcw::Engine::CellSide::East:
		case olc::rcw::Engine::CellSide::West:
			object->vel.x *= -1.0f;
			break;
		}
	}

private:
	std::string sMap;
	olc::vi2d vWorldSize;
	olc::Renderable gfxWall;
	olc::Renderable gfxFloor;
	olc::Renderable gfxbuilding;
	olc::Renderable saber;
	olc::Renderable star;
	olc::Renderable r2d2;
	olc::Renderable reticle;
	olc::Renderable reticleGlow;
};




class RayCastWorldDemo_SIMPLE : public olc::PixelGameEngine
{
public:

	RayCastWorldDemo_SIMPLE()
	{
		sAppName = "RayCastWorld - SIMPLE";
		saber.LoadFromFile("saber1.png");
		pull.LoadFromFile("pull.png");
		push.LoadFromFile("push.png");
		icon.LoadFromFile("reticle1.png");
		iconglow.LoadFromFile("reticle2.png");
	}

public:
	bool OnUserCreate() override
	{
		// Create game object
		pGame.reset(new ExampleGame(ScreenWidth(), ScreenHeight(), 3.14159f / 3.333f));

		
	
		player->pos = { 2.0f,2.0f };
		player->angle = 3.14159f / 4.0f;
		player->bVisible = false;
		player->bCollideWithObjects = true;

		pGame->mapObjects.insert_or_assign(0, player);

		
		star->nGenericID = 1;
		star->pos = { 3.0f, 12.0f };
		star->bVisible = true;
		star->bNotifySceneryCollision = true;
		star->bCollideWithObjects = true;
		star->bIsActive = true;
		star->objecthit = false;
		
		star->fRadius = 0.6f;
		//star->vel = { 3.0f,3.0f };
		pGame->mapObjects.insert_or_assign(1, star);

		//for (int i = 0; i < 100; i++)
		//{
		//	std::shared_ptr<olc::rcw::Object> r2 = std::make_shared<olc::rcw::Object>();
		//	r2->nGenericID = 2;
		//	r2->pos = { float(rand() % 64) + 0.5f, float(rand() % 32) + 0.5f };
		//	r2->fHeading = (float(rand()) / float(RAND_MAX)) * 2.0f * 3.14159f;
		//	r2->bVisible = true;
		//	r2->bNotifySceneryCollision = true;
		//	r2->bCollideWithObjects = true;
		//	r2->bCanBeMoved = true;
		//	r2->bIsActive = true;
		//	r2->fRadius = 0.3f;
		//	r2->vel = { 1.0f,1.0f };
		//
		//	pGame->mapObjects.insert_or_assign(2 + i, r2);
		//}
		 
		


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		auto& player = pGame->mapObjects[0];

		if (GetKey(olc::Key::A).bHeld)
		{
			player->Turn(-fPlayerMoveSpeed * 0.1f * fElapsedTime);
		}
		if (GetKey(olc::Key::D).bHeld)
		{
			player->Turn(+fPlayerMoveSpeed * 0.1f * fElapsedTime);
		}
		

		if (GetKey(olc::Key::LEFT).bHeld)
		{
			ispush = true;
		}
		else {
			ispush = false;
		}
		if (GetKey(olc::Key::RIGHT).bHeld)
		{
			ispull = true;
		}
		else {
			ispull = false;
		}

		if (GetKey(olc::Key::T).bHeld)
		{
			kinesis = true;
		}
		else {
			kinesis = false;
		}
		player->Stop();

		if (GetKey(olc::Key::W).bHeld) player->Walk(+fPlayerMoveSpeed);
		// Walk Backwards
		if (GetKey(olc::Key::S).bHeld) player->Walk(-fPlayerMoveSpeed);
		// Strafe Right
		if (GetKey(olc::Key::E).bHeld) player->Strafe(+fPlayerMoveSpeed);
		// Strafe Left
		if (GetKey(olc::Key::Q).bHeld) player->Strafe(-fPlayerMoveSpeed);

		pGame->Update(fElapsedTime);

		pGame->SetCamera(player->pos, player->fHeading);

		

		
		pGame->Render();
		if (ispull)
		{
			DrawSprite(120, 185, &pull);
		}
		else if(ispush) {
			DrawSprite(120, 170, &push);
		}
		else {
			DrawSprite(120, 100, &saber);
		}
		if (kinesis)
		{
			star->pos = { player->pos.x, player->pos.y };
		}
		
		float reticlelimit = 0.2356f;
		float Rfov = 0.4712f;
		float reticleRightlimit = 0.2356f;
		bool objectHit = false;


		for (auto & o : pGame->mapObjects)
		{
		
			const std::shared_ptr<olc::rcw::Object> object = o.second;
			
			olc::vf2d vObject = object->pos - player->pos;
			
			float fDistanceToObject = vObject.mag();
			float fObjectAngle = atan2f(vObject.x, vObject.y) - player->fHeading;
			
			
			
			if (fObjectAngle < 0) { fObjectAngle += 2.0f * 3.14159f; }
			if (fObjectAngle > 2.0f * 3.14159f) { fObjectAngle += 0; }
			


			//reticleraycast(objectHit,o.second);
		 DrawString(5, 5, "player->angle " + std::to_string(player->fHeading));
			//DrawString(5,14, "fObjectAngle: " + std::to_string(fObjectAngle));

			if (objectHit)
			{
				DrawSprite(140, 100, &iconglow);
			}
			else {
				DrawSprite(140, 100, &icon);
			}

		
		}
		return true;
	}
	//test

	void reticleraycast(bool &objectHit, std::shared_ptr<olc::rcw::Object> &object)
	{
		float depth = ScreenWidth();
		float reticlelimit = 0.2356f;
		float Rfov = 0.39;
		
			//raycast attempt
			for (float x = 0.1; x < Rfov; x++)
			{
				float frayangle = (player->fHeading - Rfov / 2.0f) + (float)x / Rfov;
				float nextstep = 0.1f;
				float step = 0.0f;
				
				bool boudary = false;
				float feyex = sinf(frayangle);
				float feyey = cosf(frayangle);
				while (!objectHit)
				{
			
			
					step += nextstep;
					//int fovleft = (int)(player->fHeading + eyex )
					float ntestx = player->fHeading + feyex * step;
					float ntesty = player->fHeading + feyey * step;
			
					if ((int)ntestx < 0 || (int)ntestx >= ScreenWidth() || (int)ntesty < 0 || (int)ntesty >= ScreenHeight())
					{
						break;
					}
			
					if ((int)ntestx == (int)object->pos.x && (int)ntesty == (int)object->pos.y)
					{
						objectHit = true;
						
					}
					else
					{
						objectHit = false;
					}
			
				}
			
			}
		
	}

public:
	float fPlayerMoveSpeed = 16.0f;
	std::shared_ptr<olc::rcw::Object> player = std::make_shared<olc::rcw::Object>();
	std::shared_ptr<olc::rcw::Object> star = std::make_shared<olc::rcw::Object>();
	std::unique_ptr<ExampleGame> pGame = nullptr;
	olc::Sprite saber;
	olc::Sprite pull;
	olc::Sprite push;
	olc::Sprite icon;
	olc::Sprite iconglow;
	bool kinesis = false;
	bool ispush = false;
	bool ispull = false;
	
};



int main()
{
	RayCastWorldDemo_SIMPLE demo;
	if (demo.Construct(320, 240, 4, 4))
		demo.Start();
	return 0;
}