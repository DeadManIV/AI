#ifndef BASETANK_H
#define BASETANK_H

#include "GameObject.h"
#include "Texture2D.h"
#include "Commons.h"
#include <SDL.h>
#include <vector>

using namespace::std;

//---------------------------------------------------------------

class BaseTank : public GameObject
{
	//---------------------------------------------------------------
public:
	BaseTank(SDL_Renderer* renderer, TankSetupDetails details);
	~BaseTank();

	virtual void	ChangeState(BASE_TANK_STATE newState);

	virtual void	Update(float deltaTime, SDL_Event e);
	virtual void	Render();
	Rect2D			GetAdjustedBoundingBox();

	//Attributes.
	int				GetHealth()												{return mHealth;}
	int				GetBullets()											{return mBullets;}
	int				GetRockets()											{return mRockets;}
	int				GetFuel()												{return mFuel;}
	int				GetMines()												{return mMines;}

	//Movement.
	double			GetMass()												{return mMass;}
	double			GetCurrentSpeed()										{return mCurrentSpeed;}
	double			GetMaxSpeed()											{return mMaxSpeed;}
	double			GetMaxForce()											{return mMaxForce;}
	double			GetMaxTurnRate()										{return mMaxTurnRate;}
	string			GetTankName()											{return mStudentName;}

	float			GetHearingRadius();
	float			GetNoiseRadius();

	void			SetPosition(Vector2D newPosition)						{mPosition = newPosition;}
	//Vector2D		GetPointAtFrontOfTank();
	//Vector2D		GetPointAtRearOfTank();
	
	void			GetCornersOfTank(Vector2D* topLeft, Vector2D* topRight, Vector2D* bottomLeft, Vector2D* bottomRight);

	void			IncrementTankRotationAngle(double deg);
	void			IncrementManRotationAngle(double deg);

	void			DeductABullet()											{mBullets--;}
	void			AddBullets(int numToAdd)								{mBullets += numToAdd;}
	void			DeductAMine()											{mMines--;}
	void			AddMines(int numToAdd)									{mMines += numToAdd;}
	void			DeductARocket()											{mRockets--;}
	void			AddRockets(int numToAdd)								{mRockets += numToAdd;}
	void			TakeDamage(GAMEOBJECT_TYPE projectile);
	void			AddHealth(int numToAdd)									{mHealth += numToAdd;}

	void			AddToScore(SCORE_TYPE scoreType);							
	void			OutputScoreDetails();

	//void			Rebound(Vector2D position);

	Vector2D		GetHeading()											{return mHeading;}
	Vector2D		GetVelocity()											{return mVelocity;}
	Vector2D		GetSide()												{return mSide;}
	
	bool			CanSee(BaseTank* tank);
	bool			IsAlive()												{return mAlive;}

	void			Explode()												{mAlive = false; mExploding = true; mExplosionFrame = 0;}
	bool			IsExploding()											{return mExploding;}
	bool			HasExploded()											{return (mExploding && mExplosionFrame >= kNumberOfSpritesPerExplosion);}			
	//---------------------------------------------------------------
protected:
	virtual void	MoveInHeadingDirection(float deltaTime);

	bool			RotateHeadingToFacePosition(Vector2D target, float deltaTime);
	virtual void	RotateHeadingByRadian(double radian, int sign, float deltaTime);		//Sign is direction of turn. Either -1 or 1.
	void			RotateManByRadian(double radian, int sign, float deltaTime);

	void			SetHeading(Vector2D newHeading);

	void			FireABullet();
	void			FireRockets();
	void			DropAMine();

	//---------------------------------------------------------------
private:
	SDL_Rect		GetCurrentManSprite();
	SDL_Rect		GetCurrentCannonSprite();
	SDL_Rect		GetCurrentExplosionSprite();

	void			DrawFoV();
	void			DrawDebugCircle(Vector2D centrePoint, float radius, int red, int green, int blue);
	void			DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue);

	//---------------------------------------------------------------
private:
	//We need this to pass on to bullets.
	SDL_Renderer*		mRenderer;

	TANK_TYPE			mTankType;

	//Animating man in tank.
	Texture2D*			mManSpritesheet;					//The man in the tank.
	int					mManSingleSpriteHeight;
	int					mManSingleSpriteWidth;

	bool				mManFireFrame;
	float				mManFireTime;

	float				mBulletDelay;
	float				mRocketDelay;
	float				mMineDelay;
	float				mExplosionDelay;

	Vector2D			mManOffset;
	double				mManRotationAngle;

	//Animating Cannon details.
	Texture2D*			mCannonSpritesheet;					//The man in the tank.
	int					mCannonSingleSpriteHeight;
	int					mCannonSingleSpriteWidth;
	bool				mCannonAttachedRight;
	bool				mCannonAttachedLeft;
	bool				mCannonFireFrame;
	float				mCannonFireTime;
	bool				mFiringRocket;

	Texture2D*			mExplosionSpritesheet;				//The explosion.
	int					mExplosionSingleSpriteHeight;
	int					mExplosionSingleSpriteWidth;
	int					mExplosionFrame;
	float				mExplosionTime;
	bool				mExploding;
	Vector2D			mExplosionOffset;

	//Identifying details.
	string				mStudentName;

	//Attributes.
	int					mHealth;
	int					mBullets;
	int					mFuel;
	int					mMines;
	int					mRockets;
	
	float				mScore;
	int					mNumberOfBulletHits;
	int					mNumberOfRocketHits;
	int					mNumberOfMineHits;
	int					mNumberOfDestroyedTanks;
	int					mNumberOfSurvivalBonus;
	int					mNumberOfPickUps;

	bool				mAlive;

	//---------------------------------------------------------------
protected:
	BASE_TANK_STATE		mCurrentState;

	//Movement.
	double				mCurrentSpeed;
	Vector2D			mVelocity;
	Vector2D			mHeading;
	Vector2D			mSide;	
	
	double				mMass;
	double				mMaxSpeed;
	double				mMaxForce;
	double				mMaxTurnRate;	

	float				mHearingRadius;
	float				mNoiseRadius;
	float				mGunfireNoiseAffect;

	Vector2D			mManFireDirection;

	vector<BaseTank*>	mTanksICanSee;
	vector<BaseTank*>	mTanksICanHear;
};

//---------------------------------------------------------------

#endif //BASETANK_H