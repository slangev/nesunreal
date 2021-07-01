// Fill out your copyright notice in the Description page of Project Settings.

#include "NesCart.h"

NesCart::NesCart()
{
    mbc = std::make_unique<NesNoMapper>();
}

NesCart::~NesCart()
{
}
