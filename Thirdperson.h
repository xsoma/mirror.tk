#pragma once

// class vector;

class thirdperson : public singleton<thirdperson>
{
public:
	void run();
	void set_tp_angle(const vector tp_angle) const;
	vector& get_tp_angle() const { return tp_angle; }
private:
	mutable vector tp_angle;
};