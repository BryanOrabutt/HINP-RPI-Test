#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <sched.h>
#include <stdlib.h>
#include <pthread.h>
#include "hinp_rpi.h"
#include <signal.h>
#include <time.h>

/* Setup GTK Object handles for each widget */

//global settings check boxes
GtkWidget *GEN_CB_h;
GtkWidget *GMode_CB_h;
GtkWidget *Neg_Pol_CB_h;
GtkWidget *Internal_AGND_CB_h;
GtkWidget *Autopeak_CB_h;
GtkWidget *Sel_Shaper_CB_h;
GtkWidget *Odd_Pulser_CB_h;
GtkWidget *Even_Pulser_CB_h;

//global settings combo boxes/text boxes
GtkWidget *Nowlin_Mode_Menu_h;
GtkWidget *Nowlin_Delay_Menu_h;
GtkWidget *AR_Digital_Menu_h;
GtkWidget *Auto_Reset_Menu_h;
GtkWidget *Lockout_DAC_Box_h;
GtkWidget *AGND_Trim_Menu_h;
GtkWidget *TVC_Buff_Menu_h;
GtkWidget *LG_Buff_Menu_h;
GtkWidget *HG_Buff_Menu_h;
GtkWidget *TVC_Mode_Menu_h;

//channel enable checkboxes
GtkWidget *Channel0_EN_CB_h;
GtkWidget *Channel1_EN_CB_h;
GtkWidget *Channel2_EN_CB_h;
GtkWidget *Channel3_EN_CB_h;
GtkWidget *Channel4_EN_CB_h;
GtkWidget *Channel5_EN_CB_h;
GtkWidget *Channel6_EN_CB_h;
GtkWidget *Channel7_EN_CB_h;
GtkWidget *Channel8_EN_CB_h;
GtkWidget *Channel9_EN_CB_h;
GtkWidget *Channel10_EN_CB_h;
GtkWidget *Channel11_EN_CB_h;
GtkWidget *Channel12_EN_CB_h;
GtkWidget *Channel13_EN_CB_h;
GtkWidget *Channel14_EN_CB_h;
GtkWidget *Channel15_EN_CB_h;

//leading edge DAC text boxes
GtkWidget *Channel0_LE_DAC_Box_h;
GtkWidget *Channel1_LE_DAC_Box_h;
GtkWidget *Channel2_LE_DAC_Box_h;
GtkWidget *Channel3_LE_DAC_Box_h;
GtkWidget *Channel4_LE_DAC_Box_h;
GtkWidget *Channel5_LE_DAC_Box_h;
GtkWidget *Channel6_LE_DAC_Box_h;
GtkWidget *Channel7_LE_DAC_Box_h;
GtkWidget *Channel8_LE_DAC_Box_h;
GtkWidget *Channel9_LE_DAC_Box_h;
GtkWidget *Channel10_LE_DAC_Box_h;
GtkWidget *Channel11_LE_DAC_Box_h;
GtkWidget *Channel12_LE_DAC_Box_h;
GtkWidget *Channel13_LE_DAC_Box_h;
GtkWidget *Channel14_LE_DAC_Box_h;
GtkWidget *Channel15_LE_DAC_Box_h;

//leading edge DAC sign bit text boxes.
GtkWidget *Channel0_Sign_CB_h;
GtkWidget *Channel1_Sign_CB_h;
GtkWidget *Channel2_Sign_CB_h;
GtkWidget *Channel3_Sign_CB_h;
GtkWidget *Channel4_Sign_CB_h;
GtkWidget *Channel5_Sign_CB_h;
GtkWidget *Channel6_Sign_CB_h;
GtkWidget *Channel7_Sign_CB_h;
GtkWidget *Channel8_Sign_CB_h;
GtkWidget *Channel9_Sign_CB_h;
GtkWidget *Channel10_Sign_CB_h;
GtkWidget *Channel11_Sign_CB_h;
GtkWidget *Channel12_Sign_CB_h;
GtkWidget *Channel13_Sign_CB_h;
GtkWidget *Channel14_Sign_CB_h;
GtkWidget *Channel15_Sign_CB_h;

//ADC text boxes
GtkWidget *Channel0_ADC_Box_h;
GtkWidget *Channel1_ADC_Box_h;
GtkWidget *Channel2_ADC_Box_h;
GtkWidget *Channel3_ADC_Box_h;
GtkWidget *Channel4_ADC_Box_h;
GtkWidget *Channel5_ADC_Box_h;
GtkWidget *Channel6_ADC_Box_h;
GtkWidget *Channel7_ADC_Box_h;
GtkWidget *Channel8_ADC_Box_h;
GtkWidget *Channel9_ADC_Box_h;
GtkWidget *Channel10_ADC_Box_h;
GtkWidget *Channel11_ADC_Box_h;
GtkWidget *Channel12_ADC_Box_h;
GtkWidget *Channel13_ADC_Box_h;
GtkWidget *Channel14_ADC_Box_h;
GtkWidget *Channel15_ADC_Box_h;

//Save & load interface handles
GtkWidget *Save_File_Box_h;
GtkWidget *Load_File_Box_h;
GtkWidget *Save_Config_Button_h;
GtkWidget *Load_Config_Button_h;

//config button
GtkWidget *Confiure_Button_h;

//RST_L button & Force Reset Button
GtkWidget *RST_Button_h;
GtkWidget *FR_Button_h;

//Read Registers button
GtkWidget *Read_Register_Button_h;

//ACQ All button
GtkWidget *ACQ_All_Button_h;

//Experiment and output features
GtkWidget *Start_Experiment_Button_h;
GtkWidget *Num_Events_Box_h;
GtkWidget *Out_File_Box_h;


/* Global variables */
unsigned int leading_edge_dac[CHANNELS]; //value to write to individual channel
char gmode;
char neg_pol;
char gen;
char int_agnd_en;
char agnd_trim;
char nowlin_mode;
char nowlin_delay;
char autopeak;
char odd_pulser;
char even_pulser;
char sel_shaper;
char ar_digital;
char auto_reset;
char tvc_buffer;
char hg_buffer;
char lg_buffer;
char tvc_mode;
unsigned int lockout_dac;
char ch_en[CHANNELS]; //individual channel enable flags
char ch_sign[CHANNELS]; //individual channel DAC sign bit flags
char wait_flag;


/* Callback functions */

/* Grabs the DAC text box values and converts to integer, storing
*  in a variable for saving/loading/config.
*/
void grab_dacs()
{
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel0_LE_DAC_Box_h)), "%i", leading_edge_dac+0);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel1_LE_DAC_Box_h)), "%i", leading_edge_dac+1);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel2_LE_DAC_Box_h)), "%i", leading_edge_dac+2);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel3_LE_DAC_Box_h)), "%i", leading_edge_dac+3);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel4_LE_DAC_Box_h)), "%i", leading_edge_dac+4);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel5_LE_DAC_Box_h)), "%i", leading_edge_dac+5);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel6_LE_DAC_Box_h)), "%i", leading_edge_dac+6);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel7_LE_DAC_Box_h)), "%i", leading_edge_dac+7);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel8_LE_DAC_Box_h)), "%i", leading_edge_dac+8);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel9_LE_DAC_Box_h)), "%i", leading_edge_dac+9);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel10_LE_DAC_Box_h)), "%i", leading_edge_dac+10);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel11_LE_DAC_Box_h)), "%i", leading_edge_dac+11);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel12_LE_DAC_Box_h)), "%i", leading_edge_dac+12);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel13_LE_DAC_Box_h)), "%i", leading_edge_dac+13);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel14_LE_DAC_Box_h)), "%i", leading_edge_dac+14);
	sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Channel15_LE_DAC_Box_h)), "%i", leading_edge_dac+15);
}

/* When GEN is toggled, change the gen variable and print a message */
void on_GEN_CB_toggled()
{
	printf("gen_cb = %p\n", GEN_CB_h);
	gen = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GEN_CB_h))) ? 1:0;
	set_gen(gen);
	printf("GEN toggled: %s\n", (gen) ? "ON":"OFF");
}

/* Sets the GUI state for Gmode as described in GMode_CB_toggled
*/
void gmode_helper()
{
	if(gmode)
	{
			gtk_entry_set_text(GTK_ENTRY(Channel1_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel1_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel1_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel2_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel2_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel2_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel3_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel3_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel3_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel4_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel4_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel4_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel5_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel5_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel5_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel6_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel6_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel6_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel7_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel7_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel7_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel8_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel8_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel8_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel9_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel9_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel9_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel10_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel10_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel10_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel11_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel11_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel11_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel12_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel12_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel12_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel13_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel13_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel13_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel14_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel14_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel14_LE_DAC_Box_h, 0);
			gtk_entry_set_text(GTK_ENTRY(Channel15_LE_DAC_Box_h), "N/A");
			gtk_editable_set_editable(GTK_EDITABLE(Channel15_LE_DAC_Box_h), 0);
			gtk_widget_set_can_focus(Channel15_LE_DAC_Box_h, 0);
	}
	else
	{
			gtk_entry_set_text(GTK_ENTRY(Channel1_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel1_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel1_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel2_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel2_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel2_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel3_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel3_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel3_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel4_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel4_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel4_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel5_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel5_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel5_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel6_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel6_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel6_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel7_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel7_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel7_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel8_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel8_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel8_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel9_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel9_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel9_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel10_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel10_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel10_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel11_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel11_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel11_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel12_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel12_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel12_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel13_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel13_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel13_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel14_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel14_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel14_LE_DAC_Box_h, 1);
			gtk_entry_set_text(GTK_ENTRY(Channel15_LE_DAC_Box_h), "");
			gtk_editable_set_editable(GTK_EDITABLE(Channel15_LE_DAC_Box_h), 1);
			gtk_widget_set_can_focus(Channel15_LE_DAC_Box_h, 1);
	}

}

/* Gen Gmode is toggled, change gmode variable, then update alll channel DAC
 * text fields so that they are uneditable and display "N/A". Allow only
 * channel 0 to be updated.
*/
void on_GMode_CB_toggled()
{
	GtkToggleButton* gmode_cb = GTK_TOGGLE_BUTTON(GMode_CB_h);

	gmode = (gtk_toggle_button_get_active(gmode_cb)) ? 1:0;
	gmode_helper();
	
	printf("GMode toggled: %s\n", (gmode) ? "ON":"OFF");
}

/* When Neg_Pol is toggled, update neg_pol variable and print a message */
void on_Neg_Pol_CB_toggled()
{
	GtkToggleButton* neg_pol_cb = GTK_TOGGLE_BUTTON(Neg_Pol_CB_h);

	neg_pol = (gtk_toggle_button_get_active(neg_pol_cb)) ? 1:0;
	printf("Neg pol toggled: %s\n", (neg_pol) ? "ON":"OFF");
}

/* When Autopeak is toggled, update autopeak variable and print a message */
void on_Autopeak_CB_toggled()
{
	GtkToggleButton* autopeak_cb = GTK_TOGGLE_BUTTON(Autopeak_CB_h);

	autopeak = (gtk_toggle_button_get_active(autopeak_cb)) ? 1:0;
	printf("Autopeak toggled: %s\n", (autopeak) ? "ON":"OFF");
}

/* When Sel shaper is toggled, update sel_shaper variable and print a message */
void on_Sel_Shaper_CB_toggled()
{
	GtkToggleButton* sel_shaper_cb = GTK_TOGGLE_BUTTON(Sel_Shaper_CB_h);

	sel_shaper = (gtk_toggle_button_get_active(sel_shaper_cb)) ? 1:0;
	printf("Sel shaper toggled: %s\n", (sel_shaper) ? "ON":"OFF");
}

/* When Odd pulser is toggled, update odd_pulser variable and print a message */
void on_Odd_Pulser_CB_toggled()
{
	GtkToggleButton* odd_pulser_cb = GTK_TOGGLE_BUTTON(Odd_Pulser_CB_h);

	odd_pulser = (gtk_toggle_button_get_active(odd_pulser_cb)) ? 1:0;
	printf("Odd pulser toggled: %s\n", (odd_pulser) ? "ON":"OFF");
}

/* When Even pulser is toggled, update even_pulser variable and print a message */
void on_Even_Pulser_CB_toggled()
{
	GtkToggleButton* even_pulser_cb = GTK_TOGGLE_BUTTON(Even_Pulser_CB_h);

	even_pulser = (gtk_toggle_button_get_active(even_pulser_cb)) ? 1:0;
	printf("Even Pulser toggled: %s\n", (even_pulser) ? "ON":"OFF");
}

/* When Int_AGND is toggled, update int_agnd_en variable and print a message */
void on_Internal_AGND_CB_toggled()
{
	GtkToggleButton* agnd_cb = GTK_TOGGLE_BUTTON(Internal_AGND_CB_h);

	int_agnd_en = (gtk_toggle_button_get_active(agnd_cb)) ? 1:0;
	set_internal_agnd(int_agnd_en);
	printf("Internal AGND toggled: %s\n", (int_agnd_en) ? "ON":"OFF");
}

/* When Nowlin_Mode_Menu changes state, determine what state is has changed
 * to, and update the values in the NowlinDelay box. This is done by delteing
 * all current values in the selection list and appending new ones to match 
 * the mode selected.
*/
void on_Nowlin_Mode_Menu_changed()
{
	printf("Nowlin mode menu changed\t");
	GtkComboBoxText* delaybox = GTK_COMBO_BOX_TEXT(Nowlin_Delay_Menu_h);
	GtkComboBoxText* modebox = GTK_COMBO_BOX_TEXT(Nowlin_Mode_Menu_h);
	
	gchar* mode = gtk_combo_box_text_get_active_text(modebox);
	g_printf("%s\n", mode);

	gtk_combo_box_text_remove_all(delaybox);

	nowlin_mode = (char)gtk_combo_box_get_active(GTK_COMBO_BOX(modebox));
	int scale = (nowlin_mode == NOWLIN_LONG) ? 12:1;
	nowlin_mode = (nowlin_mode == NOWLIN_LONG) ? 0:1;
	printf("Nowlin mode changed to: %s\n", (nowlin_mode == NOWLIN_SHORT) ? "SHORT":"LONG");

	for(int i = 0; i < CHANNELS; i++)
	{
		gchar str[4];
		g_snprintf(str, 4, "%d", (i+1)*scale);
		gtk_combo_box_text_append_text(delaybox, str);
	}	

	gtk_combo_box_set_active(GTK_COMBO_BOX(delaybox), 0);

}

/* When Nowlin_Delay changes, save delay menu index into nowlin_delay 
 * variable and print a message
*/
void on_Nowlin_Delay_Menu_changed()
{
	GtkComboBoxText* delaybox = GTK_COMBO_BOX_TEXT(Nowlin_Delay_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(delaybox));
	gchar* delay = gtk_combo_box_text_get_active_text(delaybox);

	nowlin_delay = (char)index;

	g_printf("Nowlin delay menu changed: %s ns\tIndex: %d\n", delay, nowlin_delay);
}

/* When AR Digital changes, save delay menu index into ar_digital
 * variable and print a message
*/
void on_AR_Digital_Menu_changed()
{
	GtkComboBoxText* ardigital_box = GTK_COMBO_BOX_TEXT(AR_Digital_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(ardigital_box));
	gchar* val = gtk_combo_box_text_get_active_text(ardigital_box);

	ar_digital = (char)index;

	g_printf("Digital autoreset menu changed: %s ns\tIndex: %d\n", val, ar_digital);
}

/* When Auto reset changes, save delay menu index into auto_reset
 * variable and print a message
*/
void on_Auto_Reset_Menu_changed()
{
	GtkComboBoxText* auto_reset_box = GTK_COMBO_BOX_TEXT(Auto_Reset_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(auto_reset_box));
	gchar* val = gtk_combo_box_text_get_active_text(auto_reset_box);

	auto_reset = (char)index;

	g_printf("Analog autoreset menu changed: %s ns\tBit Val: 0x%01X\n", val, (char)~auto_reset);
}

/* When TVC buffer changes, save delay menu index into tvc_buff
 * variable and print a message
*/
void on_TVC_Buff_Menu_changed()
{
	GtkComboBoxText* buff_box = GTK_COMBO_BOX_TEXT(TVC_Buff_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(buff_box));
	gchar* val = gtk_combo_box_text_get_active_text(buff_box);

	tvc_buffer = (char)index;

	g_printf("TVC buffer menu changed: %s ns\tIndex: %d\n", val, tvc_buffer);
}

/* When HG buffer changes, save delay menu index into hg_buffer
 * variable and print a message
*/
void on_HG_Buff_Menu_changed()
{
	GtkComboBoxText* buff_box = GTK_COMBO_BOX_TEXT(HG_Buff_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(buff_box));
	gchar* val = gtk_combo_box_text_get_active_text(buff_box);

	hg_buffer = (char)index;

	g_printf("HG buffer menu changed: %s ns\tIndex: %d\n", val, hg_buffer);
}

/* When LG Buffer changes, save delay menu index into lg_buffer
 * variable and print a message
*/
void on_LG_Buff_Menu_changed()
{
	GtkComboBoxText* buff_box = GTK_COMBO_BOX_TEXT(LG_Buff_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(buff_box));
	gchar* val = gtk_combo_box_text_get_active_text(buff_box);

	lg_buffer = (char)index;

	g_printf("LG buffer menu changed: %s ns\tIndex: %d\n", val, lg_buffer);
}

/* When TVC mode changes, save menu index into tvc_mode
 * variable and print a message
*/
void on_TVC_Mode_Menu_changed()
{
	GtkComboBoxText* mode_box = GTK_COMBO_BOX_TEXT(TVC_Mode_Menu_h);
	
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(mode_box));
	gchar* val = gtk_combo_box_text_get_active_text(mode_box);

	tvc_mode = (char)index;

	g_printf("TVC mode menu changed: %s ns\tIndex: %d\n", val, tvc_mode);
}
/* When AGND_Trim menu changes, save index into agnd_trim variable and
 * print a message.
*/
void on_AGND_Trim_Menu_changed()
{
	GtkComboBoxText* agnd_box = GTK_COMBO_BOX_TEXT(AGND_Trim_Menu_h);
	gchar* val = gtk_combo_box_text_get_active_text(agnd_box);

	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(agnd_box));
	agnd_trim = (char)index;

	g_printf("AGND trim menu changed: %s\n Index = %d\n", val, index);
}

/* Master callback for all ChannelX_EN_CB callbacks. Print message and toggle
 * the ch_en variable for the channel
*/
void channel_enable_event(int channel)
{
	printf("Channel%i enable toggled\n", channel);

	ch_en[channel] ^= 1;	

	printf("ch_en[%d]: %d\n", channel, ch_en[channel]);

}

void on_Channel0_EN_CB_toggled()
{
	channel_enable_event(0);
}

void on_Channel1_EN_CB_toggled()
{
	channel_enable_event(1);
}

void on_Channel2_EN_CB_toggled()
{
	channel_enable_event(2);
}

void on_Channel3_EN_CB_toggled()
{
	channel_enable_event(3);
}

void on_Channel4_EN_CB_toggled()
{
	channel_enable_event(4);
}

void on_Channel5_EN_CB_toggled()
{
	channel_enable_event(5);
}

void on_Channel6_EN_CB_toggled()
{
	channel_enable_event(6);
}

void on_Channel7_EN_CB_toggled()
{
	channel_enable_event(7);
}

void on_Channel8_EN_CB_toggled()
{
	channel_enable_event(8);
}

void on_Channel9_EN_CB_toggled()
{
	channel_enable_event(9);
}

void on_Channel10_EN_CB_toggled()
{
	channel_enable_event(10);
}

void on_Channel11_EN_CB_toggled()
{
	channel_enable_event(11);
}

void on_Channel12_EN_CB_toggled()
{
	channel_enable_event(12);
}

void on_Channel13_EN_CB_toggled()
{
	channel_enable_event(13);
}

void on_Channel14_EN_CB_toggled()
{
	channel_enable_event(14);
}

void on_Channel15_EN_CB_toggled()
{
	channel_enable_event(15);
}

/* Master callback for all ChannelX_Sign_CB callbacks. Print message and toggle
 * the ch_en variable for the channel
*/
void channel_sign_event(int channel)
{
	printf("Channel%i DAC sign bit toggled\n", channel);

	ch_sign[channel] ^= 1;	

	printf("ch_sign[%d]: %d\n", channel, ch_sign[channel]);

}

void on_Channel0_Sign_CB_toggled()
{
	channel_sign_event(0);
}

void on_Channel1_Sign_CB_toggled()
{
	channel_sign_event(1);
}

void on_Channel2_Sign_CB_toggled()
{
	channel_sign_event(2);
}

void on_Channel3_Sign_CB_toggled()
{
	channel_sign_event(3);
}

void on_Channel4_Sign_CB_toggled()
{
	channel_sign_event(4);

}

void on_Channel5_Sign_CB_toggled()
{
	channel_sign_event(5);
}

void on_Channel6_Sign_CB_toggled()
{
	channel_sign_event(6);
}

void on_Channel7_Sign_CB_toggled()
{
	channel_sign_event(7);
}

void on_Channel8_Sign_CB_toggled()
{
	channel_sign_event(8);
}

void on_Channel9_Sign_CB_toggled()
{
	channel_sign_event(9);
}

void on_Channel10_Sign_CB_toggled()
{
	channel_sign_event(10);
}

void on_Channel11_Sign_CB_toggled()
{
	channel_sign_event(11);
}

void on_Channel12_Sign_CB_toggled()
{
	channel_sign_event(12);
}

void on_Channel13_Sign_CB_toggled()
{
	channel_sign_event(13);
}

void on_Channel14_Sign_CB_toggled()
{
	channel_sign_event(14);
}

void on_Channel15_Sign_CB_toggled()
{
	channel_sign_event(15);
}


/* When Configure_Button is clicked the CFD chip will be configured.
*  Config starts with common channel (mode 1, mode 0, and mode 5).
*  Config ends with signal channel DAC register (mode 6)
*/
void on_Configure_Button_clicked()
{
    char data = 0;
    
    set_gen(0);
    
    //configure data registers
    //register0
    set_read();
    delay_ns(500);
    strobe_low();
    delay_ns(500);
	set_write();
    delay_ns(500);
    set_data(0);
    printf("Selecting REG0\n");
    strobe_high();
    
    data |= even_pulser;
    printf("Setting even pusler bit: %d\n", even_pulser);
    data |= odd_pulser << 1;
    printf("Setting odd pusler bit: %d\n", odd_pulser);
    data |= nowlin_delay << 2;
    printf("Setting nowlin delay value to: %d\n", nowlin_delay);
    data |= nowlin_mode << 6;
    printf("Setting nowlin mode bit to: %d\n", nowlin_mode);
    data |= ((hg_buffer == BUFF_50) || (hg_buffer == BUFF_NEG_50)) ? 0:(1 << 7);
    printf("Setting HG buffer bias level bit to: %d\n", ((hg_buffer == BUFF_50) || (hg_buffer == BUFF_NEG_50)));
    
    //data = 0xaa;
    
    set_data(data);
    delay_ns(500);
    strobe_low();
    delay_ns(500);
    
    //register 1
    printf("Selecting REG1\n");
    set_data(1);
    delay_ns(500);
    strobe_high();
    delay_ns(500);
    
    data = 0;
    data |= ((hg_buffer == BUFF_NEG_50) || (hg_buffer == BUFF_NEG_25)) ? 1:0;
    printf("Setting HG buffer polarity bit: %d\n", ((hg_buffer == BUFF_NEG_50) || (hg_buffer == BUFF_NEG_25)));
    data |= ((lg_buffer == BUFF_50) || (lg_buffer == BUFF_NEG_50)) ? 0:(1 << 1);
    printf("Setting LG buffer bias level bit to: %d\n", !((lg_buffer == BUFF_50) || (lg_buffer == BUFF_NEG_50)));
    data |= ((lg_buffer == BUFF_NEG_50) || (lg_buffer == BUFF_NEG_25)) ? 1:0;
    printf("Setting LG buffer polarity bit to: %d\n", ((lg_buffer == BUFF_NEG_50) || (lg_buffer == BUFF_NEG_25)));
    data |= autopeak << 3;
    printf("Setting autopeak enable bit to: %d\n", autopeak);
    data |= sel_shaper << 4;
    printf("Setting diagnostic mode bit (select shaper mode) to: %d\n", sel_shaper);
    data |= agnd_trim << 5;
    printf("Setting AGND trim bits to: %d\n", agnd_trim);
    
    //data = 0x00;
    set_data(data);
    delay_ns(500);
    strobe_low();
    delay_ns(500);
    
    //register 2
    printf("Selecting REG2\n");
    set_data(2);
    delay_ns(500);
    strobe_high();
    delay_ns(500);
    
    data = 0;
    data |= tvc_mode;
    printf("Setting TVC mode bit to: %d\n", tvc_mode);
    data |= (neg_pol == COLLECT_HOLES) ? (1 << 2):0;
    printf("Setting holes/electrons collection to: %s\n", (neg_pol) ? "HOLES":"ELECTRONS");
    data |= ~auto_reset << 3;
    printf("Setting analog auto reset bits to: %d\n", auto_reset);
    data |= ar_digital << 7;
    printf("Setting digital auto reset bit to: %d\n", ar_digital);
    
    //data = 7 << 3;
    delay_ns(500);
    set_data(data);
    delay_ns(500);
    strobe_low();

    int iter = 0;
	grab_dacs(); //get dac box data

    do
	{
		//Configure channel registers.	
		//set mode 6
		data = (gmode << 3);
		data |= 6;
		data |= (iter << 4);
		printf("addr_mode = 0x%02X\n", data);
		set_data(data);
		delay_ns(500);
	
		strobe_high();
		delay_ns(500);

		//set data for channel
		data = 0;

		leading_edge_dac[iter] &= 0x1f; //clear top 3 MSBs since they should not be used.

		data = leading_edge_dac[iter];
		data |= (ch_sign[iter] << 5);
		data |= (ch_en[iter] << 6);
		//data = 0x21;
		set_data(data);
		printf("Writing DAC%d data = 0x%02X\n", iter, data);
		delay_ns(500);

		strobe_low();
		iter++;	
	
		if(iter > 15) break;
	
	} while(!gmode);
	
	delay_ns(500);
    set_data(7);
    delay_ns(500);
    strobe_high();
    set_gen(1);
    set_read();
    strobe_low();
    
	g_printf("Configuration done!\n");
	//set_gen(gen);
    

	
}

/* When Save_Config is clicked, open a file using named specified in the
 * Save_File_Box and write every global variable to the file
*/
void on_Save_Config_Button_clicked()
{
	GtkEntry* save_file = GTK_ENTRY(Save_File_Box_h); //GTK object handle for save text box
	const gchar* filename = gtk_entry_get_text(save_file); //get file name from text box

	
	/* Open file for writing and verify no errors */
	FILE* fd = fopen((const char*)filename, "w"); 

	if(!fd)
	{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	fwrite(&gmode, sizeof(gmode), 1, fd);
	fwrite(&neg_pol, sizeof(gmode), 1, fd);
	fwrite(&gen, sizeof(gen), 1, fd);
	fwrite(&int_agnd_en, sizeof(int_agnd_en), 1, fd);
	fwrite(&agnd_trim, sizeof(agnd_trim), 1, fd);
	fwrite(&nowlin_mode, sizeof(nowlin_mode), 1, fd);
	fwrite(&nowlin_delay, sizeof(nowlin_delay), 1, fd);
	fwrite(&autopeak, sizeof(autopeak), 1, fd);
	fwrite(&odd_pulser, sizeof(odd_pulser), 1, fd);
	fwrite(&even_pulser, sizeof(even_pulser), 1, fd);
	fwrite(&sel_shaper, sizeof(sel_shaper), 1, fd);
	fwrite(&ar_digital, sizeof(ar_digital), 1, fd);
	fwrite(&auto_reset, sizeof(auto_reset), 1, fd);
	fwrite(&tvc_buffer, sizeof(tvc_buffer), 1, fd);
	fwrite(&hg_buffer, sizeof(hg_buffer), 1, fd);
	fwrite(&lg_buffer, sizeof(lg_buffer), 1, fd);
	fwrite(&tvc_mode, sizeof(tvc_mode), 1, fd);
	
	grab_dacs();
	fwrite(leading_edge_dac, sizeof(unsigned int), CHANNELS, fd);
	fwrite(ch_en, sizeof(char), CHANNELS, fd);
	fwrite(ch_sign, sizeof(char), CHANNELS, fd);
	
	fclose(fd);
	
	printf("Saved configuration data to %s.\n", filename);
}

/* When Load_Config is clicked, open a file using named specified in the
 * Load_File_Box and read contents out, updating each global variable.
*/
void on_Load_Config_Button_clicked()
{
	GtkEntry* load_file = GTK_ENTRY(Load_File_Box_h); //GTK object handle for load text box
	const gchar* filename = gtk_entry_get_text(load_file); //read file name from text box
	
	/* Attempt to open file for read */
	FILE* fd = fopen((const char*)filename, "r");
	gchar str[7];

	if(!fd)
	{
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	/* Load configuration data and update GUI */
	fread(&gmode, sizeof(gmode), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(GMode_CB_h), gmode);	
	fread(&neg_pol, sizeof(gmode), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Neg_Pol_CB_h), neg_pol);	
	fread(&gen, sizeof(gen), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(GEN_CB_h), gen);	
	fread(&int_agnd_en, sizeof(int_agnd_en), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(GEN_CB_h), gen);	
	fread(&agnd_trim, sizeof(agnd_trim), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(AGND_Trim_Menu_h), agnd_trim);
	fread(&nowlin_mode, sizeof(nowlin_mode), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(Nowlin_Mode_Menu_h), nowlin_mode);
	fread(&nowlin_delay, sizeof(nowlin_delay), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(Nowlin_Delay_Menu_h), nowlin_delay);
	fread(&autopeak, sizeof(autopeak), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Autopeak_CB_h), autopeak);
	fread(&odd_pulser, sizeof(odd_pulser), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Odd_Pulser_CB_h), odd_pulser);
	fread(&even_pulser, sizeof(even_pulser), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Even_Pulser_CB_h), even_pulser);
	fread(&sel_shaper, sizeof(sel_shaper), 1, fd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Sel_Shaper_CB_h), sel_shaper);
	fread(&ar_digital, sizeof(ar_digital), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(AR_Digital_Menu_h), ar_digital);
	fread(&auto_reset, sizeof(auto_reset), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(Auto_Reset_Menu_h), auto_reset);
	fread(&tvc_buffer, sizeof(tvc_buffer), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(TVC_Buff_Menu_h), tvc_buffer);
	fread(&hg_buffer, sizeof(hg_buffer), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(HG_Buff_Menu_h), hg_buffer);
	fread(&lg_buffer, sizeof(lg_buffer), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(LG_Buff_Menu_h), lg_buffer);
	fread(&tvc_mode, sizeof(tvc_mode), 1, fd);
	gtk_combo_box_set_active(GTK_COMBO_BOX(TVC_Mode_Menu_h), tvc_mode);
	
	fread(leading_edge_dac, sizeof(unsigned int), CHANNELS, fd);
	
	for(int j = 0; j < 16; j++)
		printf("DAC%d: 0x%02X\n", j, leading_edge_dac[j]);
	
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[0]);
	gtk_entry_set_text(GTK_ENTRY(Channel0_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[1]);
	gtk_entry_set_text(GTK_ENTRY(Channel1_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[2]);
	gtk_entry_set_text(GTK_ENTRY(Channel2_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[3]);
	gtk_entry_set_text(GTK_ENTRY(Channel3_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[4]);
	gtk_entry_set_text(GTK_ENTRY(Channel4_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[5]);
	gtk_entry_set_text(GTK_ENTRY(Channel5_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[6]);
	gtk_entry_set_text(GTK_ENTRY(Channel6_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[7]);
	gtk_entry_set_text(GTK_ENTRY(Channel7_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[8]);
	gtk_entry_set_text(GTK_ENTRY(Channel8_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[9]);
	gtk_entry_set_text(GTK_ENTRY(Channel9_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[10]);
	gtk_entry_set_text(GTK_ENTRY(Channel10_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[11]);
	gtk_entry_set_text(GTK_ENTRY(Channel11_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[12]);
	gtk_entry_set_text(GTK_ENTRY(Channel12_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[13]);
	gtk_entry_set_text(GTK_ENTRY(Channel13_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[14]);
	gtk_entry_set_text(GTK_ENTRY(Channel14_LE_DAC_Box_h), str);
	g_snprintf(str, 6, "0x%02X", leading_edge_dac[15]);
	gtk_entry_set_text(GTK_ENTRY(Channel15_LE_DAC_Box_h), str);
	
	fread(ch_sign, sizeof(char), CHANNELS, fd);
	char temp[16];
	for(int j = 0; j < 16; j++)
		temp[j] = ch_sign[j];
		
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel0_Sign_CB_h), ch_sign[0]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel1_Sign_CB_h), ch_sign[1]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel2_Sign_CB_h), ch_sign[2]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel3_Sign_CB_h), ch_sign[3]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel4_Sign_CB_h), ch_sign[4]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel5_Sign_CB_h), ch_sign[5]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel6_Sign_CB_h), ch_sign[6]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel7_Sign_CB_h), ch_sign[7]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel8_Sign_CB_h), ch_sign[8]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel9_Sign_CB_h), ch_sign[9]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel10_Sign_CB_h), ch_sign[10]);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel11_Sign_CB_h), ch_sign[11]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel12_Sign_CB_h), ch_sign[12]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel13_Sign_CB_h), ch_sign[13]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel14_Sign_CB_h), ch_sign[14]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel15_Sign_CB_h), ch_sign[15]);
		
	for(int j = 0; j < 16; j++)
	{
		ch_sign[j] = temp[j];	
		printf("Channel%d sign bit: %d\n", j, ch_sign[j]);
	}
	
	fread(ch_en, sizeof(char), CHANNELS, fd);
	for(int j = 0; j < 16; j++)
		temp[j] = ch_en[j];
		
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel0_EN_CB_h), ch_en[0]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel1_EN_CB_h), ch_en[1]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel2_EN_CB_h), ch_en[2]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel3_EN_CB_h), ch_en[3]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel4_EN_CB_h), ch_en[4]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel5_EN_CB_h), ch_en[5]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel6_EN_CB_h), ch_en[6]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel7_EN_CB_h), ch_en[7]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel8_EN_CB_h), ch_en[8]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel9_EN_CB_h), ch_en[9]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel10_EN_CB_h), ch_en[10]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel11_EN_CB_h), ch_en[11]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel12_EN_CB_h), ch_en[12]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel13_EN_CB_h), ch_en[13]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel14_EN_CB_h), ch_en[14]);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Channel15_EN_CB_h), ch_en[15]);
	
	for(int j = 0; j < 16; j++)
	{
		ch_en[j] = temp[j];	
		printf("Channel%d enable bit: %d\n", j, ch_en[j]);
	}
	
	fclose(fd);
	
	printf("Loading configuration data from %s.\n", filename);
}

void on_Read_Register_Button_clicked()
{
	printf("Reading back registers...\n");	
	
	set_write();
	set_data(0);
	delay_ns(500);
	strobe_high();
	delay_ns(500);
	set_read();
	delay_ns(500);
	
	printf("REG0: 0x%02X\t", read_addr_dat());	
	strobe_low();
	set_write();
	
	set_write();
	set_data(1);
	delay_ns(500);
	strobe_high();
	delay_ns(500);
	set_read();
	delay_ns(500);
	
	printf("REG1: 0x%02X\t", read_addr_dat());
	strobe_low();
	
	set_write();
	set_data(2);
	delay_ns(500);

	strobe_high();
	delay_ns(500);
	set_read();
	delay_ns(500);
	
	printf("REG2: 0x%02X\n", read_addr_dat());
	strobe_low();
	
	printf("DAC Registers...\n");
	
	for(int iter = 0; iter < 16; iter++)
	{
		set_write();
		set_data((iter << 4) | 6);
		delay_ns(500);
		strobe_high();
		delay_ns(500);
		set_read();
		delay_ns(500);
	
		printf("DAC%d: 0x%02X\n", iter, read_addr_dat());	
		strobe_low();	
		delay_ns(500);
		set_write();
	}
	
	printf("Hit Regs...\n");
	int hr = 0;
    set_write();
    set_data(4);
    delay_ns(500);
    strobe_high();
    set_read();
    strobe_low();
   	hr |= read_addr_dat();
    
    set_write();
    set_data(5);
    delay_ns(500);
    strobe_high();
    set_read();
    strobe_low();
    hr |= read_addr_dat() << 8;
    printf("Hit Regs: 0x%04X\n", hr);
	
}

void on_ACQ_All_Button_clicked()
{
	printf("Asserting ACQ_ALL for 2 us\n");
	set_acq_all(1);
	set_acq_all(0);	
}

/* Applys a 1 us high active Force Reset pulse when button is clicked */
void on_FR_Button_clicked()
{
	if(gmode)
	{
		printf("Asserting Global Force Reset\n");
		force_reset_high();	
		set_write();
		set_data(0);
		delay_ns(500);
		strobe_high();
		delay_ns(500);
		set_read();
		delay_ns(500);
		strobe_low();
		force_reset_low();
	}
	else
	{	
		printf("Asserting Force Reset for 2 us\n");
		force_reset_high();
		delay_ns(500);
		force_reset_low();
	}	
}

void on_Start_Experiment_Button_clicked()
{
	int iter;
	char data;
	printf("Beginning experiment...\n");
	if(sel_shaper)
	{
		printf("Diagnostic mode enabled.\n");
		for(iter = 0; iter < 16; iter++)
		{
			set_write();
			wait_flag = 1;
			data |= iter << 4; //load addr bits
			data |= 7; //load mode bits
			set_data(data);
			strobe_high();
			delay_ns(500);
			set_read();
			delay_ns(500);
			strobe_low();
			printf("Channel%d shaper driving buffer\n", iter);
			while(wait_flag)
			{
				
			}
		}
	}
    else
    {
		struct adc_readings adc_channel[CHANNELS] = {{0}};
		gchar out_file_name[255];
		int num_events = 0;
		printf("Setting experiment mode (mode 7) and waiting for OR interrupt!\n");
		set_write();
		set_data(7);
		delay_ns(500);
		strobe_high();
		set_read();
		strobe_low();
		
		sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Out_File_Box_h)), "%s", out_file_name);
		
		sscanf((char*)gtk_entry_get_text(GTK_ENTRY(Num_Events_Box_h)), "%d", &num_events);
		
		FILE* ofile_h = fopen(out_file_name, "w");
		
		if(num_events <= 0)
		{
			num_events = 1000000000;
		}
		
		printf("Reading %d events\n", num_events);
	
		for(int j = 0; j < num_events; j++)
		{
			wait_flag = 1;
			while(!read_or_out_pin() && wait_flag)
			{
				
			}
		
			if(!wait_flag)
			{
				break;
			}	
			
			/*if(tvc_mode == TVC_500NS)
			{
				for(int i = 0; i < 2; i++)
				{
					toggle_dummy();
				}
			}
			else
			{
				for(int i = 0; i < 3; i++)
				{
					toggle_dummy();
				}
			}*/
		
			set_common_stop(1);
			set_gen(0);

			set_write();
			delay_ns(500);
			set_data(7);
			delay_ns(500);
			strobe_high();
			delay_ns(500);
			set_read();
			delay_ns(500);
			strobe_low();
		
			set_take_event(1);
			char* is_hit = calloc(16, sizeof(char));
			
			while(read_or_out_pin())
			{
				int channel = read_addr_dat();
				is_hit[channel] = 1;
				adc_channel[channel] = read_adcs();
				strobe_high();
				force_reset_high();
				//delay_ns(500);
				strobe_low();
				force_reset_low();
				force_reset_high();
				set_data(8);
				force_reset_low();
			}
			set_take_event(0);
			set_common_stop(0);
			set_gen(1);
			printf("ADCs have been sampled!\n");
			printf("Writing event#%d to file: %s\n", j, out_file_name);
			fprintf(ofile_h, "Event %d:\nChannel\tTVC\tLG\tHG\tHIT\n", j);
			
			for(iter = 0; iter < CHANNELS; iter++)
			{
				fprintf(ofile_h, "%d\t0x%02X\t0x%02X\t0x%02X\t%d\n\n", iter, adc_channel[iter].tvc, adc_channel[iter].low_gain, adc_channel[iter].high_gain, is_hit[iter]);
			}
			fflush(ofile_h);
			free(is_hit);
			is_hit = NULL;
		}
		
		fclose(ofile_h);
		
		for(iter = 0; iter < CHANNELS; iter++)
		{
			gchar str[100];
			g_snprintf(str, 99, "tvc: 0x%04X    lg: 0x%04X    hg: 0x%04X", adc_channel[iter].tvc, adc_channel[iter].low_gain, adc_channel[iter].high_gain);
		
			switch(iter)
			{   
				case 0:
				    gtk_entry_set_text(GTK_ENTRY(Channel0_ADC_Box_h), str);
				    break;
				case 1:
				    gtk_entry_set_text(GTK_ENTRY(Channel1_ADC_Box_h), str);
				    break;
				case 2:
				    gtk_entry_set_text(GTK_ENTRY(Channel2_ADC_Box_h), str);
				    break;
				case 3:
				    gtk_entry_set_text(GTK_ENTRY(Channel3_ADC_Box_h), str);
				    break;
				case 4:
				    gtk_entry_set_text(GTK_ENTRY(Channel4_ADC_Box_h), str);
				    break;
				case 5:
				    gtk_entry_set_text(GTK_ENTRY(Channel5_ADC_Box_h), str);
				    break;
				case 6:
				    gtk_entry_set_text(GTK_ENTRY(Channel6_ADC_Box_h), str);
				    break;
				case 7:
				    gtk_entry_set_text(GTK_ENTRY(Channel7_ADC_Box_h), str);
				    break;
				case 8:
				    gtk_entry_set_text(GTK_ENTRY(Channel8_ADC_Box_h), str);
				    break;
				case 9:
				    gtk_entry_set_text(GTK_ENTRY(Channel9_ADC_Box_h), str);
				    break;
				case 10:
				    gtk_entry_set_text(GTK_ENTRY(Channel10_ADC_Box_h), str);
				    break;
				case 11:
				    gtk_entry_set_text(GTK_ENTRY(Channel11_ADC_Box_h), str);
				    break;
				case 12:
				    gtk_entry_set_text(GTK_ENTRY(Channel12_ADC_Box_h), str);
				    break;
				case 13:
				    gtk_entry_set_text(GTK_ENTRY(Channel13_ADC_Box_h), str);
				    break;
				case 14:
				    gtk_entry_set_text(GTK_ENTRY(Channel14_ADC_Box_h), str);
				    break;
				case 15:
				    gtk_entry_set_text(GTK_ENTRY(Channel15_ADC_Box_h), str);
				    break;
				default:
				    printf("Error setting ADC box entries\n");
			}
		}
		
	}
	
	
 
 	set_read();
 	delay_ns(500);
 	strobe_low();
    set_gen(gen);
}

/* Applys a 5 us low active reset pulse when button is clicked */
void on_RST_Button_clicked()
{
	printf("Applying RST_L for 5 us\n");
	pulse_rst_l(5000);
	
}

/* When main window is closed, exit program 
*/
void on_window_main_destroy()
{
	rpi_cleanup_gpio();
	gtk_main_quit();
}

void sigint_handler(int x)
{
	wait_flag = 0;
	printf("SIGINT received... Exiting loop!\n");
}

int main(int argc, char *argv[])
{
	GtkBuilder      *builder;
	GtkWidget       *window;
	struct sched_param sp;
	sp.sched_priority = 50;
	sched_setscheduler(getpid(), SCHED_FIFO, &sp);
	
	signal(SIGINT, sigint_handler);

	gtk_init(&argc, &argv);

	/* Create a builder object to read the XML file */
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "HINP.glade", NULL);

	
	/* Using builder object, create an object handle for each GUI element */
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

	GEN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "GEN_CB"));
	GMode_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "GMode_CB"));
	Neg_Pol_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Neg_Pol_CB"));
	Internal_AGND_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Internal_AGND_CB"));
	Autopeak_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Autopeak_CB"));
	Sel_Shaper_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Sel_Shaper_CB"));
	Odd_Pulser_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Odd_Pulser_CB"));
	Even_Pulser_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Even_Pulser_CB"));

	Nowlin_Mode_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "Nowlin_Mode_Menu"));
	Nowlin_Delay_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "Nowlin_Delay_Menu"));
	AR_Digital_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "AR_Digital_Menu"));
	Auto_Reset_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "Auto_Reset_Menu"));
	TVC_Buff_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "TVC_Buff_Menu"));
	LG_Buff_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "LG_Buff_Menu"));
	HG_Buff_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "HG_Buff_Menu"));
	TVC_Mode_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "TVC_Mode_Menu"));
	AGND_Trim_Menu_h = GTK_WIDGET(gtk_builder_get_object(builder, "AGND_Trim_Menu"));

	Channel0_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel0_EN_CB"));
	Channel0_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel0_LE_DAC_Box"));
	Channel0_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel0_Sign_CB"));
	Channel0_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel0_ADC_Box"));

	Channel1_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel1_EN_CB"));
	Channel1_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel1_LE_DAC_Box"));
	Channel1_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel1_Sign_CB"));
	Channel1_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel1_ADC_Box"));

	Channel2_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel2_EN_CB"));
	Channel2_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel2_LE_DAC_Box"));
	Channel2_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel2_Sign_CB"));
	Channel2_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel2_ADC_Box"));

	Channel3_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel3_EN_CB"));
	Channel3_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel3_LE_DAC_Box"));
	Channel3_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel3_Sign_CB"));
	Channel3_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel3_ADC_Box"));

	Channel4_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel4_EN_CB"));
	Channel4_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel4_LE_DAC_Box"));
	Channel4_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel4_Sign_CB"));
	Channel4_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel4_ADC_Box"));

	Channel5_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel5_EN_CB"));
	Channel5_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel5_LE_DAC_Box"));
	Channel5_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel5_Sign_CB"));
	Channel5_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel5_ADC_Box"));

	Channel6_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel6_EN_CB"));
	Channel6_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel6_LE_DAC_Box"));
	Channel6_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel6_Sign_CB"));
	Channel6_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel6_ADC_Box"));

	Channel7_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel7_EN_CB"));
	Channel7_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel7_LE_DAC_Box"));
	Channel7_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel7_Sign_CB"));
	Channel7_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel7_ADC_Box"));

	Channel8_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel8_EN_CB"));
	Channel8_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel8_LE_DAC_Box"));
	Channel8_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel8_Sign_CB"));
	Channel8_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel8_ADC_Box"));

	Channel9_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel9_EN_CB"));
	Channel9_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel9_LE_DAC_Box"));
	Channel9_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel9_Sign_CB"));
	Channel9_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel9_ADC_Box"));

	Channel10_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel10_EN_CB"));
	Channel10_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel10_LE_DAC_Box"));
	Channel10_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel10_Sign_CB"));
	Channel10_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel10_ADC_Box"));

	Channel11_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel11_EN_CB"));
	Channel11_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel11_LE_DAC_Box"));
	Channel11_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel11_Sign_CB"));
	Channel11_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel11_ADC_Box"));

	Channel12_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel12_EN_CB"));
	Channel12_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel12_LE_DAC_Box"));
	Channel12_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel12_Sign_CB"));
	Channel12_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel12_ADC_Box"));

	Channel13_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel13_EN_CB"));
	Channel13_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel13_LE_DAC_Box"));
	Channel13_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel13_Sign_CB"));
	Channel13_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel13_ADC_Box"));

	Channel14_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel14_EN_CB"));
	Channel14_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel14_LE_DAC_Box"));
	Channel14_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel14_Sign_CB"));
	Channel14_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel14_ADC_Box"));

	Channel15_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel15_EN_CB"));
	Channel15_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel15_LE_DAC_Box"));
	Channel15_Sign_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel15_Sign_CB"));
	Channel15_ADC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel15_ADC_Box"));

	Save_File_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Save_File_Name_Box"));
	Load_File_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Load_File_Name_Box"));
	Save_Config_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "Save_Config_Button"));
	Load_Config_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "Load_Config_Button"));
	RST_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "RST_Button"));
	Read_Register_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "Read_Register_Button"));
	FR_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "FR_BUTTON"));
	ACQ_All_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "ACQ_All_Button"));
	
	Start_Experiment_Button_h = GTK_WIDGET(gtk_builder_get_object(builder, "Start_Experiment_Button"));
	Num_Events_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Num_Events_Box"));
	Out_File_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Out_File_Box"));
	
	/* end GUI building */

	/* Set default values for configuration */
	gen = 1; //enable chip
	gmode = 0; //global mode disabled -- each channel DAC configured individually
	neg_pol = COLLECT_ELECTRONS; //positive polarity
	int_agnd_en = 0; //enable internal generation of AGND
	agnd_trim = AGND_1_63; //set AGND for 1.63V (nominal value)
	nowlin_mode = NOWLIN_SHORT; //nowlin short mode
	nowlin_delay = 1; //set Nowlin delay for 2 ns (switch in 1 cap at short mode)
    ar_digital = AR_DIGITAL_100;
    auto_reset = 0;
    tvc_buffer = BUFF_50;
    hg_buffer = BUFF_50;
    lg_buffer = BUFF_50;
    tvc_mode = TVC_2US;
    autopeak = 1;
    odd_pulser = 0;
    even_pulser = 0;
    sel_shaper = 0;
    ar_digital = 1;
    auto_reset = 15;
    wait_flag = 0;

	for(int i = 0; i < CHANNELS; i++)
	{
		ch_en[i] = 1; //by default enable all channels
		leading_edge_dac[i] = 0; //by default all DACs loaded with 0
		ch_sign[i] = 0; //sign bit off by default.
	}


	/* Set lockout DAC default state */
	GtkEntry* lockout_dac = GTK_ENTRY(Lockout_DAC_Box_h);	
	gtk_entry_set_text(lockout_dac, "N/A");
	gtk_editable_set_editable(GTK_EDITABLE(lockout_dac), 0);
	gtk_widget_set_can_focus(Lockout_DAC_Box_h, 0);

	/* Connect signal handlers for GUI */
	gtk_builder_connect_signals(builder, NULL);
	printf("connect returned\n");

	g_object_unref(builder);
	printf("unref returned\n");

	gtk_widget_show(window);
	printf("show returned\n");

	printf("Resetting GPIO states and applying RST_L pulse\n");

	/* Set GPIO to default states */
	rpi_setup_io();
	rpi_configure();
    	
   	set_read();
	printf("Initializing ADCs for next conversion\n");
    //init_adcs();

	//set_write();
	//int addr_dat = 0xaa;
	//set_data(addr_dat);

    //pthread_create(&or_thread, NULL, read_or_out, NULL);

	gtk_main();
    
    //pthread_join(or_thread, NULL);
	//printf("Joining pthreads\n");
	return 0;
}
