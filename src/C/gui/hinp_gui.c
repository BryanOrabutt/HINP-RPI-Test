#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "hinp_rpi.h"
#include <sched.h>
#include <stdlib.h>

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

//RST_L button
GtkWidget *RST_Button_h;

/* Global variables */
unsigned int leading_edge_dac[CHANNELS]; //value to write to individual channel
char gmode;
char neg_pol;
char gen;
char int_agnd_en;
char agnd_trim;
char nowlin_mode;
char nowlin_delay;
char ar_digital;
char auto_reset;
char tvc_buffer;
char hg_buffer;
char lg_buffer;
char tvc_mode;
unsigned int lockout_dac;
char ch_en[CHANNELS]; //individual channel enable flags
char ch_sign[CHANNELS]; //individual channel DAC sign bit flags



void* read_or_out(void* arg)
{    
	gchar str[32];
    int iter;
    char update = 0;
    char prev_state = 0;
    struct adc_readings adc_channel[CHANNELS] = {{0}};
    while(1)
    {
        if(read_or_out_pin())
        {
            set_gen(0);
            set_write();
            delay_ns(100);
            set_addr_mode(0, 7);
            delay_ns(100);
            strobe_high();
            delay_ns(500);
            strobe_low();
            set_read();
            set_take_event(1);
            
            for(iter = 0; iter < CHANNELS; iter++)
            {
                int channel = read_channel_number();
                adc_channel[channel] = read_adcs();
                strobe_high();
                force_reset_high();
                delay_ns(500);
                strobe_low();
                force_reset_low();
                set_take_event(0);
                force_reset_high();
                set_addr_mode(0, 8);
                force_reset_low();                
            }
            prev_state = 1;
        }
        else
        {
            if(prev_state) update = 1;
            prev_state = 0;
        }
        
        if(update)
        {
            update = 0;
            for(iter = 0; iter < CHANNELS; iter++)
            {
                g_snprintf(str, 31, "tvc: 0x%04X    lg: 0x%04X    hg: 0x%04X", adc_channel[iter].tvc, 
                           adc_channel[iter].low_gain, adc_channel[iter].high_gain);
                
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
    }
}

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

/* When Int_AGND is toggled, update int_agnd_en variable and print a message */
void on_Internal_AGND_CB_toggled()
{
	GtkToggleButton* agnd_cb = GTK_TOGGLE_BUTTON(Internal_AGND_CB_h);

	int_agnd_en = (gtk_toggle_button_get_active(agnd_cb)) ? 1:0;
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

	//set_read();
	
	//gmode on = 5 strobes, off = 19 strobes

	g_printf("Configuration done!\n");
	
}

/* When Save_Config is clicked, open a file using named specified in the
 * Save_File_Box and write every global variable to the file
*/
void on_Save_Config_Button_clicked()
{

	//g_printf("File saved to: %s\n", filename);
}

/* When Load_Config is clicked, open a file using named specified in the
 * Load_File_Box and read contents out, updating each global variable.
*/
void on_Load_Config_Button_clicked()
{
	
	//g_printf("Loaded configuration from file: %s\n", filename);
}

/* Applys a 5 us low active reset pulse when button is clicked */
void on_RST_Button_clicked()
{
	printf("Applying RST_L for 5 us\n");
	pulse_rst_l();	
}

/* When main window is closed, exit program 
*/
void on_window_main_destroy()
{
	rpi_cleanup_gpio();
	gtk_main_quit();
}


int main(int argc, char *argv[])
{
	GtkBuilder      *builder;
	GtkWidget       *window;
	struct sched_param sp;
	sp.sched_priority = 50;
	sched_setscheduler(getpid(), SCHED_FIFO, &sp);

	gtk_init(&argc, &argv);

	/* Create a builder object to read the XML file */
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "CFD.glade", NULL);

	
	/* Using builder object, create an object handle for each GUI element */
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

	GEN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "GEN_CB"));
	GMode_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "GMode_CB"));
	Neg_Pol_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Neg_Pol_CB"));
	Internal_AGND_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Internal_AGND_CB"));

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
    tvc_buffer = BUFF_25;
    hg_buffer = BUFF_25;
    lg_buffer = BUFF_25;
    tvc_mode = TVC_2US;

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
    	
	printf("Initializing ADCs for next conversion\n");
    	init_adcs();

	set_write();
	int addr_dat = 0xaa;
	set_data(addr_dat);
	

	gtk_main();

	return 0;
}
