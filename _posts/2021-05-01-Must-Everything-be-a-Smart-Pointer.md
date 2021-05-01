---
title: Must Everything be a Smart Pointer?
published: true
---

##[](#when-no-to-use)TL;DR

If you pass a smart pointer you include some ownership, a raw pointer doesn't. But use it for every allocation.

##[](#only-smart-pointer)Should we only use smart pointers?

During rewriting of a legacy `C` codebase to `C++` I have been asked if everything should be changed to a smart pointer. I replied with "Why should we do that?" and the response was "because we wanted to be modern". 

Oh dear. That was not what I wanted to have by stating that we want to have a modern codebase. For sure use smart pointers, where you used `new` or `malloc` and `delete` or `free` in former times. But not everywhere and not under any circumstances.

And here comes why. It has been explained for sure a few hundred times across the web, but I think it is worth explaining it one more time, so more people probably read it at some time.

##[](#unique)Unique Pointer

In case of a `std::unique_ptr` it seems to be a bit more obvious. This one clearly states, that there is only one of it, cannot be copied, only moved around. So most programmers are aware of that and use it with care. Did I say care? I mean they do not use it most of the time, because they cannot pass it to a function without moving it. But is that true? Well it depends.

As `std::unique_ptr` does not have a copy or copy-assignment constructor you can only move it. That's a fact. But there is this nice little method called `get`. Now what do we get here? Lets have a look at a (simplified) full signature `T* get() const noexcept;`. Looks like we could easily access the pointer, and pass this to functions of our choice. Nice. But wait, I wanted to have a `std::unique_ptr` and not pass it around. 

Well I think you misunderstood the concept. The `std::unique_ptr` is meant to be unique in a way, that is has exactly **one** owner at any point in time. And if the owner does not take care of the object referenced it will be deleted. The important part here is the owner. 

##[](#unique)Shared Pointer

A `std::shared_ptr` describes a object that is shared. Sharing means it has **at least one** owner but probably more. And these owners may leave the object alone at different times not synchronized. So the object must be there till the last one looses interest and the `std::shared_ptr` gets destructed.

Still, every owner could pass the raw pointer of the shared object around. The interface is like the one of the `std::unique_ptr`. And it should be used, as long as you do not want to **share ownership**. If you calling a function which does something with the shared object: pass a raw pointer. If you construct an object, that has the same lifetime as your `std::share_ptr`: pass a raw pointer. If you want somebody else to have ownership (and therefore control over the lifetime): pass a shared_ptr.


 
##[](#example)Compared to things in real life

Lets explain with using something familiar, a cellphone number. Generally speaking there is only one owner of a cellphone number at a point in time. You may pass the mobile phone with the SIM card the number is bound, but then you do not have access to the number any more. So the SIM is kind `std::unique_ptr`. (I know about multisim, but keep it simple). You still can pass your mobile phone to somebody else to make a call. It remains yours. And if you do not care anymore (your unique_ptr goes out of scope), the number is freed and you do not have to take care of it.

On the other hand, lets assume you share a flat with two other people. The flat has a landline. This means, the landline is a `std::shared_ptr`. All three of you kind of own the landline. As long as at least one of you care about it (own a shared_ptr) it won't get canceled.

