
f = open('helper.txt', "a")

for x in range(0,16):
	fstr = 'Channel'
	fstr = fstr + str(x)
	fstr2 = fstr
	fstr = fstr + '_EN_CB_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel'
	fstr = fstr + str(x)
	fstr = fstr + '_EN_CB");\n'

	fstr2 = fstr2 + '_LE_DAC_Box_h = GTK_WIDGET(gtk_builder_get_object(builder, "Channel'
	fstr2 = fstr2 + str(x)
	fstr2 = fstr2 + '_LE_DAC_Box");\n\n'
	
	f.write(fstr)
	f.write(fstr2)

f.close();
