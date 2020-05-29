// -----------------------------------------
// Constraints for tunable parameters
// ----------------------------------------

// Header
#include "Constraints.h"
#include "DeviceObjectDictionary.h"

#define NO		0	// equal to FALSE
#define YES		1	// equal to TRUE

// Constants
//
const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE] =
                                       {
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 0
                                    		   {ID_LEAK_MIN, ID_LEAK_MAX, ID_LEAK_DEF},									// 1
                                    		   {TIME_MIN, TIME_MAX, TIME_DEF},											// 2
                                    		   {PERCENT_MIN, PERCENT_MAX, PERCENT_DEF_STAB_ERR},						// 3
                                    		   {0, 0, 0},																// 4
                                    		   {0, 0, 0},																// 5
                                    		   {0, 0, 0},																// 6
                                    		   {0, 0, 0},																// 7
                                    		   {0, 0, 0},																// 8
                                    		   {0, 0, 0},																// 9
											   {0, INT16U_MAX, 0},														// 10
											   {0, INT16U_MAX, 0},														// 11
                                    		   {0, INT16U_MAX, 0},														// 12
                                    		   {0, INT16U_MAX, 0},														// 13
                                    		   {0, INT16U_MAX, 0},														// 14
                                    		   {0, INT16U_MAX, 0},														// 15
                                    		   {0, INT16U_MAX, 0},														// 16
                                    		   {0, INT16U_MAX, 0},														// 17
                                    		   {0, INT16U_MAX, 0},														// 18
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 19
                                    		   {0, INT16U_MAX, 0},														// 20
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 21
                                    		   {0, INT16U_MAX, 0},														// 22
                                    		   {0, INT16U_MAX, 0},														// 23
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 24
											   {0, INT16U_MAX, 0},														// 25
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 26
                                    		   {0, INT16U_MAX, 0},														// 27
                                    		   {0, INT16U_MAX, 0},														// 28
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 29
                                    		   {0, INT16U_MAX, 0},														// 30
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 31
                                    		   {0, INT16U_MAX, 0},														// 32
                                    		   {0, INT16U_MAX, 0},														// 33
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 34
                                    		   {0, INT16U_MAX, 0},														// 35
											   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 36
											   {0, INT16U_MAX, 0},														// 37
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 38
                                    		   {0, INT16U_MAX, 0},														// 39
                                    		   {0, INT16U_MAX, 0},														// 40
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 41
                                    		   {0, INT16U_MAX, 0},														// 42
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 43
                                    		   {0, INT16U_MAX, 0},														// 44
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 45
                                    		   {0, INT16U_MAX, 0},														// 46
											   {0, INT16U_MAX, 0},														// 47
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 48
                                    		   {0, INT16U_MAX, 0},														// 49
                                    		   {PROPORTIONAL_MIN, INT16U_MAX, PROPORTIONAL_DEF},						// 50
                                    		   {0, INT16U_MAX, 0},														// 51
                                    		   {0, 0, 0},																// 52
                                    		   {0, 0, 0},																// 53
                                    		   {0, 0, 0},																// 54
                                    		   {TIME_MIN, TIME_MAX, TIME_DEF},											// 55
                                    		   {0, 0, 0},																// 56
                                    		   {0, 0, 0},																// 57
                                    		   {0, 0, 0},																// 58
                                    		   {0, 0, 0},																// 59
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 60
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 61
                                    		   {0, 0, 0},																// 62
                                    		   {0, 0, 0},																// 63
                                    		   {0, 0, 0},																// 64
                                    		   {0, 0, 0},																// 65
                                    		   {0, 0, 0},																// 66
                                    		   {0, 0, 0},																// 67
                                    		   {0, 0, 0},																// 68
                                    		   {0, 0, 0},																// 69
                                    		   {HOLD_END_MIN, HOLD_END_MAX, HOLD_END_DEF},								// 70
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 71
                                    		   {0, 0, 0},																// 72
											   {0, 0, 0},																// 73
											   {0, 0, 0},																// 74
											   {0, 0, 0},																// 75
											   {0, 0, 0},																// 76
											   {0, 0, 0},																// 77
											   {0, 0, 0},																// 78
											   {0, 0, 0},																// 79
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 80
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, LATCH_RATE_SMALL_DEF},			// 81
                                    		   {LATCH_START_CURR_MIN, LATCH_START_CURR_MAX, LATCH_START_CURR_DEF},		// 82
                                    		   {TIME_MIN, TIME_MAX, TIME_DEF},											// 83
                                    		   {0, 0, 0},																// 84
                                    		   {0, 0, 0},																// 85
                                    		   {0, 0, 0},																// 86
                                    		   {0, 0, 0},																// 87
                                    		   {0, 0, 0},																// 88
                                    		   {0, 0, 0},																// 89
                                    		   {RG_CURRENT_MIN, RG_CURRENT_MAX, RG_CURRENT_DEF},						// 90
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 91
                                    		   {0, 0, 0},																// 92
                                    		   {0, 0, 0},																// 93
                                    		   {0, 0, 0},																// 94
                                    		   {0, 0, 0},																// 95
                                    		   {0, 0, 0},																// 96
                                    		   {0, 0, 0},																// 97
                                    		   {0, 0, 0},																// 98
                                    		   {0, 0, 0},																// 99
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 100
                                    		   {RISE_FALL_RATE_MIN, RISE_FALL_RATE_MAX, RISE_FALL_RATE_DEF},			// 101
                                    		   {0, 0, 0},																// 102
                                    		   {0, 0, 0},																// 103
                                    		   {0, 0, 0},																// 104
                                    		   {0, 0, 0},																// 105
                                    		   {0, 0, 0},																// 106
                                    		   {0, 0, 0},																// 107
                                    		   {0, 0, 0},																// 108
                                    		   {0, 0, 0},																// 109
                                    		   {0, 0, 0},																// 110
                                    		   {0, 0, 0},																// 111
                                    		   {0, 0, 0},																// 112
                                    		   {0, 0, 0},																// 113
                                    		   {0, 0, 0},																// 114
                                    		   {0, 0, 0},																// 115
                                    		   {0, 0, 0},																// 116
                                    		   {0, 0, 0},																// 117
                                    		   {0, 0, 0},																// 118
                                    		   {0, 0, 0},																// 119
                                    		   {0, 0, 0},																// 120
                                    		   {0, 0, 0},																// 121
                                    		   {0, 0, 0},																// 122
                                    		   {0, 0, 0},																// 123
                                    		   {0, 0, 0},																// 124
                                    		   {0, 0, 0},																// 125
                                    		   {0, 0, 0},																// 126
                                    		   {INT16U_MAX, 0, 0}														// 127
                                       };

const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START] =
                                      {
											   {VD_LIM_MIN, VD_LIM_MAX, VD_LIM_DEF},									// 128
											   {ID_LIM_MIN, ID_LIM_MAX, ID_LIM_DEF},									// 129
											   {VG_LIM_MIN, VG_LIM_MAX, VG_LIM_DEF},									// 130
											   {IG_LIM_MIN, IG_LIM_MAX, IG_LIM_DEF},									// 131
											   {FALSE, TRUE, TRUE},														// 132
											   {0, 0, 0},																// 133
											   {0, 0, 0},																// 134
											   {0, 0, 0},																// 135
											   {0, 0, 0},																// 136
											   {0, 0, 0},																// 137
											   {0, 0, 0},																// 138
											   {0, 0, 0},																// 139
											   {0, 0, 0},																// 140
											   {0, 0, 0},																// 141
											   {0, 0, 0},																// 142
											   {0, 0, 0},																// 143
											   {0, 0, 0},																// 144
											   {0, 0, 0},																// 145
											   {0, 0, 0},																// 146
											   {0, 0, 0},																// 147
											   {0, 0, 0},																// 148
											   {0, 0, 0},																// 149
											   {0, INT16U_MAX, 0},														// 150
											   {0, 0, 0},																// 151
											   {0, 0, 0},																// 152
											   {0, 0, 0},																// 153
											   {0, 0, 0},																// 154
											   {0, 0, 0},																// 155
											   {0, 0, 0},																// 156
											   {0, 0, 0},																// 157
											   {0, 0, 0},																// 158
											   {0, 0, 0},																// 159
											   {0, INT16U_MAX, 0},														// 160
											   {0, 0, 0},																// 161
											   {0, 0, 0},																// 162
											   {0, 0, 0},																// 163
											   {0, 0, 0},																// 164
											   {0, 0, 0},																// 165
											   {0, 0, 0},																// 166
											   {0, 0, 0},																// 167
											   {0, 0, 0},																// 168
											   {0, 0, 0},																// 169
											   {0, 0, 0},																// 170
											   {0, 0, 0},																// 171
											   {0, 0, 0},																// 172
											   {0, 0, 0},																// 173
											   {0, 0, 0},																// 174
											   {0, 0, 0},																// 175
											   {0, 0, 0},																// 176
											   {0, 0, 0},																// 177
											   {0, 0, 0},																// 178
											   {0, 0, 0},																// 179
											   {0, INT16U_MAX, 0},														// 180
											   {0, INT16U_MAX, 0},														// 181
											   {0, INT16U_MAX, 0},														// 182
											   {0, INT16U_MAX, 0},														// 183
											   {0, 0, 0},																// 184
											   {0, 0, 0},																// 185
											   {0, 0, 0},																// 186
											   {0, 0, 0},																// 187
											   {0, 0, 0},																// 188
											   {0, 0, 0},																// 189
											   {0, 0, 0},																// 190
											   {INT16U_MAX, 0, 0}														// 191
                                      };

// No more
