#pragma once
#include <math.h>

class Vec3
{
public:
	float x, y, z;

	Vec3() : x(0), y(0), z(0) {}

	Vec3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	Vec3 operator+(const Vec3& v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 operator-(const Vec3& v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
};



class clientinfo_t
{
public:
	__int32 clientnum; //0x0000 
	char pad_0x0004[0x8]; //0x0004
	__int32 teamID; //0x000C 
	char pad_0x0010[0x1FF4]; //0x0010

}; //Size=0xCC0


class refdef_t
{
public:
	char pad_0x0000[0x10]; //0x0000
	float fov_x; //0x0010 
	float fov_y; //0x0014 
	Vec3 eyePos; //0x0018 
	Vec3 axis_x; //0x0024 
	Vec3 axis_y; //0x0030 
	Vec3 axis_z; //0x003C 
	char pad_0x0048[0xBF8]; //0x0048
};

class cgs_t
{
public:
	char pad_0x0000[0x8]; //0x0000
	__int32 width; //0x0008 
	__int32 height; //0x000C 
	char pad_0x0010[0xC]; //0x0010
	__int32 servertime; //0x001C 
	char pad_0x0020[0x4]; //0x0020
	__int32 gametype; //0x0024 
	char pad_0x0028[0x1C]; //0x0028
	char servername[8]; //0x3130E9C0 
	char pad_0x004C[0xFC]; //0x004C
	__int32 maxclients; //0x0148 
	char pad_0x014C[0x6A4]; //0x014C

}; 


class cg_t
{
public:
	char pad_0x0000[0x18]; //0x0000
	__int32 isInGame; //0x0018 
	__int32 serverTime; //0x001C 
	char pad_0x0020[0x18]; //0x0020
	__int8 isSpectatingID; //0x0038 
	char pad_0x003A[0x77]; //0x003A
	Vec3 location; //0x00B0 
	char pad_0x00BC[0x164]; //0x00BC
	__int32 health; //0x0220 
	char pad_0x0224[0x4]; //0x0224
	__int32 maxHealth; //0x0228 
	char pad_0x022C[0x400]; //0x022C
	__int32 secondaryMagazine; //0x062C 
	char pad_0x0630[0x14]; //0x0630
	__int32 primaryMagazine; //0x0644 
	char pad_0x0648[0x98]; //0x0648
	__int32 secondaryAmmo; //0x06E0 
	char pad_0x06E4[0x2C]; //0x06E4
	__int32 primaryAmmo; //0x0710 
	char pad_0x0714[0x12C]; //0x0714

}; 


class centity_t
{
public:
	char pad_0x0000[0x20]; //0x0000
	__int32 x; //0x0020 
	__int32 y; //0x0024 
	__int32 z; //0x0028 
	char pad_0x002C[0xBC]; //0x002C
	Vec3 lerpOrigin; //0x00E8 
	char pad_0x00F4[0x18]; //0x00F4
	float pitch; //0x010C 
	float yaw; //0x0110 
	char pad_0x0114[0x7C]; //0x0114
	__int32 weaponID; //0x0190 
	char pad_0x0194[0x8]; //0x0194
	__int8 eFlag; //0x019C	 (0 or 2 is up, 4 or 6 is crouch, 8 or 10 is down)
	char pad_0x019D[0x3]; //0x019D
	__int32 eType; //0x01A0	 (Player: 1, Bot: 3)
	char pad_0x01A4[0x8]; //0x01A4
	Vec3 Pos2; //0x01AC 
	char pad_0x01B8[0x8C]; //0x01B8
	unsigned char isAlive; //0x0244 //The least significant bit is getting checked 1=alive, 0=dead
	char pad_0x0245[0x7]; //0x0245
	unsigned char clientnum; //0x024C 
	char pad_0x024D[0x5F3]; //0x024D

}; //Size=0x0288



class cameraInfo
{
public:
	Vec3 recoil; //0x0000 
	Vec3 position; //0x000C 
	Vec3 velocity; //0x0018 
	char pad_0x0024[0x90]; //0x0024
	float pitch; //0x00B4 
	float yaw; //0x00B8 
	char pad_0x00BC[0x784]; //0x00BC
};


class playername_t
{
public:
	char name[20]; //0x15B9E9C0 
	char pad_0x0014[0x82C]; //0x0014

}; //Size=0x080