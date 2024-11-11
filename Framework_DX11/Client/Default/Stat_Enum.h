#pragma once

enum class STAT_TYPE
{
	STAT_TYPE_FLOAT,
	STAT_TYPE_INT,
	STAT_TYPE_BOOL,
	STAT_TYPE_CELL, // <- 기본적으로 Float이나 일정 간격으로 나누어 사용하는 스탯, EX) 특수 공격용 게이지 
	STAT_TYPE_END
};

enum class STAT_INDEX
{
	STAT_HP, // <- 체력 
	STAT_STAMINA, // <- 스태미나 
	STAT_SPECIAL, // <- 특수 공격용 게이지 

	STAT_ATTACK, //
	STAT_
};