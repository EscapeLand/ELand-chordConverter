#include "myheader.h"
using namespace tinyxml2;

#define ele(x) XMLElement* x = doc.NewElement(#x)
#define txtx(x) XMLText* x##Text = doc.NewText(x)
#define txts(x,s) XMLText* x##Text = doc.NewText(#s)
#define txt(x) txts(x,x)

static void CopyNode(tinyxml2::XMLDocument *desdoc, const tinyxml2::XMLDocument *srcdoc)
{
	// Protect from evil
	if (desdoc == NULL || srcdoc == NULL)
	{
		return;
	}
	for (const XMLNode* node = srcdoc->FirstChild(); node; node = node->NextSibling()) {
		XMLNode* copy = node->DeepClone(desdoc);
		desdoc->InsertEndChild(copy);
	}
	
}

inline saveDoc::saveDoc(const char* title, const char* composer, const char* lyricist, const char* artist, const char* tabber, const char* irights)
{
	XMLDocument doc;
	doc.Parse("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<!DOCTYPE score-partwise PUBLIC \" -//Recordare//DTD MusicXML 2.0 Partwise//EN\" \"/musicxml20/partwise.dtd\">");
	XMLElement* root = doc.NewElement("score-partwise");
	doc.InsertEndChild(root);

	ele(work);
	XMLElement* workTitle = doc.NewElement("work-title");
	txtx(title);
	work->InsertEndChild(titleText);
	root->InsertEndChild(work);

	ele(identification);

	XMLElement* creator_composer = doc.NewElement("creator");
	creator_composer->SetAttribute("type", "composer");
	XMLText* composerText = doc.NewText(composer);
	creator_composer->InsertEndChild(composerText);

	XMLElement* creator_lyricist = doc.NewElement("lyricist");
	creator_lyricist->SetAttribute("type", "lyricist");
	XMLText* lyricistText = doc.NewText(lyricist);
	creator_lyricist->InsertEndChild(lyricistText);

	XMLElement* creator_artist = doc.NewElement("artist");
	creator_artist->SetAttribute("type", "artist");
	XMLText* artistText = doc.NewText(artist);
	creator_artist->InsertEndChild(artistText);

	XMLElement* creator_tabber = doc.NewElement("tabber");
	creator_tabber->SetAttribute("type", "tabber");
	XMLText* tabberText = doc.NewText(tabber);
	creator_tabber->InsertEndChild(tabberText);

	ele(rights);
	txtx(irights);
	rights->InsertEndChild(irightsText);

	ele(encoding);
	ele(software);
	XMLText* GP = doc.NewText("Guitar Pro");
	software->InsertEndChild(GP);
	encoding->InsertEndChild(software);

	identification->InsertEndChild(creator_tabber);
	identification->InsertEndChild(creator_lyricist);
	identification->InsertEndChild(creator_artist);
	identification->InsertEndChild(creator_tabber);
	identification->InsertEndChild(rights);
	identification->InsertEndChild(encoding);
	root->InsertEndChild(identification);

	XMLElement* part_list = doc.NewElement("part-list");
	XMLElement* score_part = doc.NewElement("score-part");
	score_part->SetAttribute("id", "p0");
	XMLElement* part_name = doc.NewElement("part-name");
	XMLText* G = doc.NewText("Guitar");
	part_name->InsertEndChild(G);
	score_part->InsertEndChild(part_name);

	XMLElement* part_abbreviation = doc.NewElement("part-abbreviation");
	XMLText* S_Gt = doc.NewText("S-Gt");
	part_abbreviation->InsertEndChild(S_Gt);
	score_part->InsertEndChild(part_abbreviation);

	XMLElement* score_instrument = doc.NewElement("score-instrument");
	score_instrument->SetAttribute("id", "i0");
	score_part->InsertEndChild(score_instrument);

	XMLElement* midi_instrument = doc.NewElement("midi-instrument");
	midi_instrument->SetAttribute("id", "i0");
	XMLElement* midi_channel = doc.NewElement("midi-channel");
	XMLElement* midi_program = doc.NewElement("midi-program");
	ele(volume);
	ele(pan);
	txts(midi_channel, 1);
	txts(midi_program, 26);
	txts(volume, 0);
	txts(pan, -90);
	midi_channel->InsertEndChild(midi_channelText);
	midi_program->InsertEndChild(midi_programText);
	volume->InsertEndChild(volumeText);
	pan->InsertEndChild(panText);
	midi_instrument->InsertEndChild(midi_channel);
	midi_instrument->InsertEndChild(midi_program);
	midi_instrument->InsertEndChild(volume);
	midi_instrument->InsertEndChild(pan);
	score_part->InsertEndChild(midi_instrument);

	XMLElement* part = doc.NewElement("part");
	part->SetAttribute("id", "p0");
	root->InsertEndChild(part);
	CopyNode(&backup, &doc);
}
inline int saveDoc::save(const char* xmlPath) {
	return backup.SaveFile(xmlPath);
}
inline void saveDoc::saveMeasure(measure toSave) {
	XMLDocument doc;
	CopyNode(&doc,&backup);
	XMLElement* part = doc.FirstChildElement("score-partwise")->FirstChildElement("part");
	XMLElement* measure = doc.NewElement("measure");
	part->InsertEndChild(measure);
	char a[4];
	_itoa_s(toSave.id, a, 10);
	measure->SetAttribute("number", a);

	ele(attributes);
	XMLElement* divisions = doc.NewElement("divisions");
	XMLText* l024 = doc.NewText("1024");
	divisions->InsertEndChild(l024);
	attributes->InsertEndChild(divisions);

	XMLElement* key = doc.NewElement("key");
	XMLElement* fifths = doc.NewElement("fifths");
	XMLElement* mode = doc.NewElement("mode");
	
	_itoa_s(toSave.key.fifth, a, 10);
	XMLText* fifthsText = doc.NewText(a);
	XMLText* modeText = doc.NewText(toSave.key.mode);
	fifths->InsertEndChild(fifthsText);
	mode->InsertEndChild(modeText);
	key->InsertEndChild(fifths);
	key->InsertEndChild(mode);
	attributes->InsertEndChild(key);

	XMLElement* time = doc.NewElement("time");
	XMLElement* beats = doc.NewElement("beats");
	XMLElement* beat_type = doc.NewElement("beat-type");
	_itoa_s(toSave.time.beats, a, 10);
	XMLText* beatsText = doc.NewText(a);
	_itoa_s(toSave.time.beat_type, a, 10);
	XMLText* beat_typeText = doc.NewText(a);
	beats->InsertEndChild(beatsText);
	beat_type->InsertEndChild(beat_typeText);
	time->InsertEndChild(beats);
	time->InsertEndChild(beat_type);
	attributes->InsertEndChild(time);

	ele(clef);
	ele(sign);
	ele(line);
	txt(TAB);
	txts(l6, 6);
	sign->InsertEndChild(TABText);
	line->InsertEndChild(l6Text);
	clef->InsertEndChild(sign);
	clef->InsertEndChild(line);

	XMLElement* staff_details = doc.NewElement("staff-details");
	XMLElement* staff_lines = doc.NewElement("staff-lines");
	staff_lines->InsertEndChild(l6Text);
	//±ê×¼µ÷ÏÒ

	XMLElement* staff_tuning1 = doc.NewElement("staff-tuning");
	staff_tuning1->SetAttribute("line", "1");
	XMLElement* tuning_step1 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave1 = doc.NewElement("tuning-octave");
	txt(E);
	txts(l2, 2);
	tuning_step1->InsertEndChild(EText);
	tuning_octave1->InsertEndChild(l2Text);					//E2
	staff_tuning1->InsertEndChild(tuning_step1);
	staff_tuning1->InsertEndChild(tuning_octave1);

	XMLElement* staff_tuning2 = doc.NewElement("staff-tuning");
	staff_tuning2->SetAttribute("line", "2");
	XMLElement* tuning_step2 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave2 = doc.NewElement("tuning-octave");
	txt(A);
	tuning_step2->InsertEndChild(AText);
	tuning_octave2->InsertEndChild(l2Text);					//A2
	staff_tuning1->InsertEndChild(tuning_step2);
	staff_tuning1->InsertEndChild(tuning_octave2);

	XMLElement* staff_tuning3 = doc.NewElement("staff-tuning");
	staff_tuning3->SetAttribute("line", "3");
	XMLElement* tuning_step3 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave3 = doc.NewElement("tuning-octave");
	txt(D);
	txts(l3, 3);
	tuning_step3->InsertEndChild(DText);
	tuning_octave3->InsertEndChild(l3Text);					//D3
	staff_tuning3->InsertEndChild(tuning_step3);
	staff_tuning3->InsertEndChild(tuning_octave3);

	XMLElement* staff_tuning4 = doc.NewElement("staff-tuning");
	staff_tuning4->SetAttribute("line", "4");
	XMLElement* tuning_step4 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave4 = doc.NewElement("tuning-octave");
	txt(G);
	tuning_step4->InsertEndChild(GText);
	tuning_octave4->InsertEndChild(l3Text);					//G3
	staff_tuning4->InsertEndChild(tuning_step4);
	staff_tuning4->InsertEndChild(tuning_octave4);

	XMLElement* staff_tuning5 = doc.NewElement("staff-tuning");
	staff_tuning5->SetAttribute("line", "5");
	XMLElement* tuning_step5 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave5 = doc.NewElement("tuning-octave");
	txt(B);
	tuning_step5->InsertEndChild(BText);
	tuning_octave5->InsertEndChild(l3Text);					//B3
	staff_tuning5->InsertEndChild(tuning_step5);
	staff_tuning5->InsertEndChild(tuning_octave5);

	XMLElement* staff_tuning6 = doc.NewElement("staff-tuning");
	staff_tuning6->SetAttribute("line", "6");
	XMLElement* tuning_step6 = doc.NewElement("tuning-step");
	XMLElement* tuning_octave6 = doc.NewElement("tuning-octave");
	txts(l4, 4);
	tuning_step6->InsertEndChild(EText);
	tuning_octave6->InsertEndChild(l4Text);					//E4
	staff_tuning6->InsertEndChild(tuning_step6);
	staff_tuning6->InsertEndChild(tuning_octave6);

	staff_details->InsertEndChild(staff_lines);
	staff_details->InsertEndChild(staff_tuning1);
	staff_details->InsertEndChild(staff_tuning2);
	staff_details->InsertEndChild(staff_tuning3);
	staff_details->InsertEndChild(staff_tuning4);
	staff_details->InsertEndChild(staff_tuning5);
	staff_details->InsertEndChild(staff_tuning6);

	attributes->InsertEndChild(staff_details);

	XMLElement** notes = new XMLElement*[toSave.notes.size()];
	XMLElement** pitchs = new XMLElement*[toSave.notes.size()];
	XMLElement** steps = new XMLElement*[toSave.notes.size()];
	XMLElement** octaves = new XMLElement*[toSave.notes.size()];
	XMLElement** durations = new XMLElement*[toSave.notes.size()];
	XMLElement** voices = new XMLElement*[toSave.notes.size()];
	XMLElement** types = new XMLElement*[toSave.notes.size()];
	XMLElement** notations = new XMLElement*[toSave.notes.size()];
	XMLElement** technicals = new XMLElement*[toSave.notes.size()];
	XMLElement** frets = new XMLElement*[toSave.notes.size()];
	XMLElement** strings = new XMLElement*[toSave.notes.size()];
	XMLText** dur = new XMLText*[toSave.notes.size()];
	XMLText** str = new XMLText*[toSave.notes.size()];
	XMLText** fre = new XMLText*[toSave.notes.size()];
	XMLElement* chords = doc.NewElement("chord");
	ele(dynamics);
	ele(f);
	ele(voice);
	txts(l0, 0);
	voice->InsertEndChild(l0Text);

	txt(quarter);
	txt(eighth);
	txts(l6th, 16th);
	txts(l32th, 32th);
	txts(l64th, 64th);

	measure->InsertEndChild(attributes);
	for (size_t i = 0; i < toSave.notes.size(); i++) {
		notes[i] = doc.NewElement("note");
		pitchs[i] = doc.NewElement("pitch");
		steps[i] = doc.NewElement("step");
		octaves[i] = doc.NewElement("octave");
		durations[i] = doc.NewElement("duration");
		voices[i] = doc.NewElement("voice");
		types[i] = doc.NewElement("type");
		notations[i] = doc.NewElement("notation");
		strings[i] = doc.NewElement("string");
		frets[i] = doc.NewElement("fret");
		technicals[i] = doc.NewElement("technical");

		if (toSave.notes[i].chord) notes[i]->InsertEndChild(chords);
		switch (toSave.notes[i].notation.technical.string)
		{
		case 1:							//E2
			steps[i]->InsertEndChild(EText);
			octaves[i]->InsertEndChild(l2Text);
			break;
		case 2:							//A2
			steps[i]->InsertEndChild(AText);
			octaves[i]->InsertEndChild(l2Text);
			break;
		case 3:							//D3
			steps[i]->InsertEndChild(DText);
			octaves[i]->InsertEndChild(l3Text);
			break;
		case 4:							//G3
			steps[i]->InsertEndChild(GText);
			octaves[i]->InsertEndChild(l3Text);
			break;
		case 5:							//B3
			steps[i]->InsertEndChild(BText);
			octaves[i]->InsertEndChild(l3Text);
			break;
		case 6:							//E4
			steps[i]->InsertEndChild(EText);
			octaves[i]->InsertEndChild(l4Text);
			break;
		}
		pitchs[i]->InsertEndChild(steps[i]);
		pitchs[i]->InsertEndChild(octaves[i]);
		notes[i]->InsertEndChild(pitchs[i]);

		//duration: 4:1024

		char a[5];
		_itoa_s(256 * toSave.notes[i].timeValue, a, 10);
		dur[i] = doc.NewText(a);
		durations[i]->InsertEndChild(dur[i]);
		notes[i]->InsertEndChild(voice);

		switch (toSave.notes[i].timeValue)
		{
		case 4:
			types[i]->InsertEndChild(quarterText);
			break;
		case 8:
			types[i]->InsertEndChild(eighthText);
			break;
		case 16:
			types[i]->InsertEndChild(l6thText);
			break;
		case 32:
			types[i]->InsertEndChild(l32thText);
			break;
		case 64:
			types[i]->InsertEndChild(l64thText);
			break;
		default:
			std::cerr << "timeValue unexpected value: " << (int)toSave.notes[i].timeValue << std::endl;
			break;
		}
		notes[i]->InsertEndChild(types[i]);

		_itoa_s(toSave.notes[i].notation.technical.string, a, 10);
		str[i] = doc.NewText(a);
		strings[i]->InsertEndChild(str[i]);
		_itoa_s(toSave.notes[i].notation.technical.fret, a, 10);
		fre[i] = doc.NewText(a);
		frets[i]->InsertEndChild(doc.NewText(a));

		technicals[i]->InsertEndChild(strings[i]);
		technicals[i]->InsertEndChild(frets[i]);
		notations[i]->InsertEndChild(technicals[i]);

		dynamics->InsertEndChild(f);
		notations[i]->InsertEndChild(dynamics);
		notes[i]->InsertEndChild(notations[i]);
		measure->InsertEndChild(notes[i]);
	}
	backup.Clear();
	CopyNode(&backup, &doc);
	delete[] notes;
	delete[] pitchs;
	delete[] steps;
	delete[] octaves;
	delete[] durations;
	delete[] voices;
	delete[] types;
	delete[] notations;
	delete[] technicals;
	delete[] frets;
	delete[] strings;
	delete[] dur;
	delete[] str;
	delete[] fre;
}