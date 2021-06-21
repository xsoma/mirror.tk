#pragma once

#include "Hacks.h"
#include "Entities.h"

#include <array>
#include <deque>

#pragma once

#define DISABLE_INTERPOLATION 0
#define ENABLE_INTERPOLATION 1

class C_Tick_Record
{
public:
	explicit C_Tick_Record() : sequence(0), entity_flags(0), simulation_time(0), lower_body_yaw(0), cycle(0)
	{
	}

	~C_Tick_Record()
	{
	}

	/*
	* reset
	* Resets the tick record
	*/
	vector origin;
	vector abs_origin;
	vector velocity;
	vector object_mins;
	vector object_maxs;
	vector head_positon;
	vector body_positon;
	vector eye_angles;
	vector abs_eye_angles;

	vector m_obb_mins;
	vector m_obb_maxs;

	int sequence;
	int entity_flags;
	int ticks;
	int bones;

	int m_cached_count;
	int m_writable_bones;

	float simulation_time;
	float lower_body_yaw;
	float cycle;

	CBaseAnimState * anim;
	std::array<float, 24> pose_paramaters;

	matrix3x4 m_bone_matrix[128];

	bool built;
	bool data_filled = false;
	void reset()
	{
		if (data_filled)
			return;

		origin.clearVekt();
		velocity.clearVekt();
		object_mins.clearVekt();
		object_maxs.clearVekt();
		head_positon.clearVekt();
		body_positon.clearVekt();
		eye_angles.clearVekt();
		abs_eye_angles.clearVekt();

		sequence = 0;
		entity_flags = 0;
		ticks = -1.f;

		simulation_time = 0.f;
		lower_body_yaw = 0.f;
		cycle = 0.f;

		fill(begin(pose_paramaters), end(pose_paramaters), 0.f);

		data_filled = false;
	}

	matrix3x4_t matrixes[128];
};

/*
* C_Simulation_Data
* Used when simulating ticks for a player (useful for storing temp data)
*/
class C_Simulation_Data
{
public:
	C_Simulation_Data() : entity(nullptr), on_ground(false)
	{
	}

	~C_Simulation_Data()
	{
	}

	IClientEntity* entity;

	vector origin;
	vector velocity;

	bool on_ground;

	bool data_filled = false;
};

/*
* C_Player_Record
* Used to store ticks from a player and also information about the player
*/
class C_Player_Record
{
public:
	C_Player_Record() : entity(nullptr), tick_count(0), being_lag_compensated(false), lower_body_yaw(0), last_lower_body_yaw_last_update(0)
	{
	}

	~C_Player_Record()
	{
	}

	/*
	* reset
	* Resets the player
	*/
	void reset()
	{
		entity = nullptr;
		tick_count = -1;
		hitbox.clearVekt();
		body_position.clearVekt();
		eye_angles.clearVekt();
		being_lag_compensated = false;
		lower_body_yaw_resolved = false;
		lower_body_yaw = 0.f;
		last_lower_body_yaw_last_update = 0.f;
		simtime = 0;
		restore_record.reset();

		if (!records->empty())
			records->clear();
	}

	IClientEntity* entity;
	C_Tick_Record restore_record;
	int tick_count;
	vector hitbox;
	vector body_position;
	vector eye_angles;
	bool being_lag_compensated;
	bool lower_body_yaw_resolved = false;
	float lower_body_yaw_resolved_yaw;
	float lower_body_yaw, last_lower_body_yaw_last_update;
	float simtime = 0.f;

	std::deque<C_Tick_Record> records[12];
};

/*
* C_LagCompensation
* Used for rolling back entities and correcting players positions sent from the server
*/

#include "singleton.h"

class C_LagCompensation : public singleton<C_LagCompensation>
{
public:
	void frame_net_update_end() const;

	void handle_layers(IClientEntity * player, bool replace, bool copy);

	static bool should_lag_compensate(IClientEntity* entity);
	static void set_interpolation_flags(IClientEntity* entity, int flag);
	static float get_interpolation();
	bool is_time_delta_too_large(C_Tick_Record wish_record) const;
	void update_player_record_data(IClientEntity* entity) const;
	static void store_record_data(IClientEntity* entity, C_Tick_Record* record_data);
	static void apply_record_data(IClientEntity* entity, C_Tick_Record* record_data);
	static void simulate_movement(C_Simulation_Data* data);
	void predict_player(IClientEntity* entity, C_Tick_Record* current_record, C_Tick_Record* next_record) const;
	bool get_lowerbodyyaw_update_tick(IClientEntity* entity, C_Tick_Record* tick_record, int* record_index) const;
	int start_lag_compensation(IClientEntity* entity, int wish_tick, C_Tick_Record* output_record = nullptr) const;
	void start_position_adjustment() const;
	void start_position_adjustment(IClientEntity* entity) const;
	void finish_position_adjustment() const;
	static void finish_position_adjustment(IClientEntity* entity);
	bool is_valid(int tick);
	static void reset();
	C_Player_Record player_lag_records[64];

};

extern C_LagCompensation * lagcomp;
