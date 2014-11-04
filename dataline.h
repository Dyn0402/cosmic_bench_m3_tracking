#ifndef dataline_h
#define dataline_h

class DataLine{
	public:
		DataLine();
		~DataLine();
		void ntohs_();
		bool is_final_trailer() const;
		bool is_first_line() const;
		bool is_data() const;
		bool is_channel_ID() const;
		int get_channel_ID() const;
		int get_dream_ID() const;
		float get_data() const;
		unsigned short int data;
};

#endif