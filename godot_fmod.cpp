/*************************************************************************/
/*  godot_fmod.cpp                                                       */
/*************************************************************************/
/*                                                                       */
/*       FMOD Studio module and bindings for the Godot game engine       */
/*                                                                       */
/*************************************************************************/
/* Copyright (c) 2019 Alex Fonseka                                       */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "godot_fmod.h"

void Fmod::init(int numOfChannels, int studioFlags, int flags) {
	checkErrors(FMOD::Studio::System::create(&system));
	// initialize FMOD Studio and FMOD Low Level System with provided flags
	if (checkErrors(system->initialize(numOfChannels, studioFlags, flags, nullptr))) {
		printf("FMOD Sound System successfully initialized with %d channels\n", numOfChannels);
		if (studioFlags == FMOD_STUDIO_INIT_LIVEUPDATE)
			printf("Live update enabled!\n");
	} else
		fprintf(stderr, "FMOD Sound System failed to initialize\n");
}

void Fmod::update() {
	// update all event positions
	// update listener position
	checkErrors(system->update());
}

void Fmod::shutdown() {
	checkErrors(system->unloadAll());
	checkErrors(system->release());
}

String Fmod::loadbank(const String &pathToBank, int flags) {
	if (banks.has(pathToBank)) return pathToBank; // bank is already loaded
	FMOD::Studio::Bank *bank = nullptr;
	checkErrors(system->loadBankFile(pathToBank.ascii().get_data(), flags, &bank));
	if (bank) {
		banks.insert(pathToBank, bank);
		return pathToBank;
	}
	return pathToBank;
}

void Fmod::unloadBank(const String &pathToBank) {
	if (!banks.has(pathToBank)) return; // bank is not loaded
	auto bank = banks.find(pathToBank);
	if (bank) checkErrors(bank->value()->unload());
}

int Fmod::getBankLoadingState(const String &pathToBank) {
	if (!banks.has(pathToBank)) return -1; // bank is not loaded
	auto bank = banks.find(pathToBank);
	if (bank) {
		FMOD_STUDIO_LOADING_STATE state;
		checkErrors(bank->value()->getLoadingState(&state));
		return state;
	}
	return -1;
}

// helper function to check for errors
int Fmod::checkErrors(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		fprintf(stderr, "FMOD Sound System: %s\n", FMOD_ErrorString(result));
		return 0;
	}
	return 1;
}

void Fmod::_bind_methods() {
	ClassDB::bind_method(D_METHOD("system_init", "num_of_channels", "studio_flags", "flags"), &Fmod::init);
	ClassDB::bind_method(D_METHOD("system_update"), &Fmod::update);
	ClassDB::bind_method(D_METHOD("system_shutdown"), &Fmod::shutdown);

	ClassDB::bind_method(D_METHOD("bank_load", "path_to_bank", "flags"), &Fmod::loadbank);
	ClassDB::bind_method(D_METHOD("bank_unload", "path_to_bank"), &Fmod::unloadBank);
	ClassDB::bind_method(D_METHOD("bank_get_loading_state", "path_to_bank"), &Fmod::getBankLoadingState);

	/* FMOD_INITFLAGS */
	BIND_CONSTANT(FMOD_INIT_NORMAL);
	BIND_CONSTANT(FMOD_INIT_STREAM_FROM_UPDATE);
	BIND_CONSTANT(FMOD_INIT_MIX_FROM_UPDATE);
	BIND_CONSTANT(FMOD_INIT_3D_RIGHTHANDED);
	BIND_CONSTANT(FMOD_INIT_CHANNEL_LOWPASS);
	BIND_CONSTANT(FMOD_INIT_CHANNEL_DISTANCEFILTER);
	BIND_CONSTANT(FMOD_INIT_PROFILE_ENABLE);
	BIND_CONSTANT(FMOD_INIT_VOL0_BECOMES_VIRTUAL);
	BIND_CONSTANT(FMOD_INIT_GEOMETRY_USECLOSEST);
	BIND_CONSTANT(FMOD_INIT_PREFER_DOLBY_DOWNMIX);
	BIND_CONSTANT(FMOD_INIT_THREAD_UNSAFE);
	BIND_CONSTANT(FMOD_INIT_PROFILE_METER_ALL);
	BIND_CONSTANT(FMOD_INIT_DISABLE_SRS_HIGHPASSFILTER);

	/* FMOD_STUDIO_INITFLAGS */
	BIND_CONSTANT(FMOD_STUDIO_INIT_NORMAL);
	BIND_CONSTANT(FMOD_STUDIO_INIT_LIVEUPDATE);
	BIND_CONSTANT(FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS);
	BIND_CONSTANT(FMOD_STUDIO_INIT_SYNCHRONOUS_UPDATE);
	BIND_CONSTANT(FMOD_STUDIO_INIT_DEFERRED_CALLBACKS);
	BIND_CONSTANT(FMOD_STUDIO_INIT_LOAD_FROM_UPDATE);

	/* FMOD_STUDIO_LOAD_BANK_FLAGS */
	BIND_CONSTANT(FMOD_STUDIO_LOAD_BANK_NORMAL);
	BIND_CONSTANT(FMOD_STUDIO_LOAD_BANK_NONBLOCKING);
	BIND_CONSTANT(FMOD_STUDIO_LOAD_BANK_DECOMPRESS_SAMPLES);

	/* FMOD_STUDIO_LOADING_STATE */
	BIND_CONSTANT(FMOD_STUDIO_LOADING_STATE_UNLOADING);
	BIND_CONSTANT(FMOD_STUDIO_LOADING_STATE_LOADING);
	BIND_CONSTANT(FMOD_STUDIO_LOADING_STATE_LOADED);
	BIND_CONSTANT(FMOD_STUDIO_LOADING_STATE_ERROR);


}

Fmod::Fmod() {
	FMOD::Studio::System *system = nullptr;
}

Fmod::~Fmod() {
	Fmod::shutdown();
}