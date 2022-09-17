// Fill out your copyright notice in the Description page of Project Settings.


#include "NesTriangle.h"

NesTriangle::NesTriangle()
{
}

NesTriangle::~NesTriangle()
{
}

void NesTriangle::Tick(){

}

void NesTriangle::QuarterFrameTick() {

}

void NesTriangle::HalfFrameTick() {

}

void NesTriangle::Write(unsigned short Address, uint8 Data) {
    
}

void NesTriangle::Enabled(bool bEnabled) {
    this->bChannelEnabled = bEnabled;
}

bool NesTriangle::GateCheck() {
    return true;
}

bool NesTriangle::LengthAboveZero() {
    return true;
}

int NesTriangle::GetOutputVol() {
    return 0;
}