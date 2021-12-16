// Fill out your copyright notice in the Description page of Project Settings.


#include "NesTestCart.h"

NesTestCart::NesTestCart()
{
}

NesTestCart::NesTestCart(vector<uint8> rom) {
    this->memory = rom;
}

NesTestCart::~NesTestCart()
{
}

uint8 NesTestCart::Read(unsigned short addr) {
    return memory.at(addr & 0x7FFF);
}

void NesTestCart::Write(unsigned short addr, uint8 data) {
    memory[addr] = data;
}