/*
 * StorageInterface.h
 *
 *  Created on: Dec 20, 2017
 *      Author: Jose Andres Grau Martinez
 */

#ifndef STORAGEINTERFACE_H
#define STORAGEINTERFACE_H

#include <string>
#include <connexionRFID.h>

int StorageConfigureRFID_StartUp();

int StorageConfigureRFID_Restart();

int StorageReadUID(int StorageNumber);

std::string StorageGetStoredUID(int StorageNumber);

int StorageCleanPalletMemory(int StorageNumber);

int StorageReadAllMemory(int StorageNumber);

int StorageFormatMemory(int StorageNumber);

int StorageAddBrick(int StorageNumber,int Grade, int Colour);

int StorageEditBrick(int StorageNumber,int Position,int Grade, int Colour);

int StorageInsertBrick(int StorageNumber, int Position, int Grade, int Colour);

int StorageGetNumberOfBricks(int StorageNumber);

int StorageDeleteBrick(int StorageNumber, int Position);

bool StorageGetStoredUIDChangedFlag(int StorageNumber);

bool StorageClearStoredUIDChangedFlag(int StorageNumber);

int StorageGetStoredUIDChangedCount(int StorageNumber);

int StorageGetRaw(int StorageNumber,int Position);

int StorageGetBrickColor(int StorageNumber,int Position);

int StorageGetBrickGrade(int StorageNumber,int Position);

#endif /* STORAGEINTERFACE_H */
