---
title: unique_ptr are great
published: true
---

Sidenote: Ok, so I couldn't and will not try to stick to a plan of posting, I will post if I am in the mood of telling somebody something. 

I recently got in touch with rust, and read about it. Some things look nice, others rather strange. And a few of the "oh this is much better in C++" maybe true, others are just not so obvious.
One special case I recently stumbled over was file access. You open a file, write to it and at some point int time you are closing it. 

Well if you remember to close it, right. Maybe you forget about it, so you have it open, even if it is not used anymore. That's bad. But can be avoided. And C++ offers you a nice and clean way of doing it:

```cpp
void writeToFileUsingUniquePtr(){
  std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen("test2.txt", "w+"), fclose);
  if (fp){
    fwrite("11234\n", 1, 6, fp.get());
    std::cout << "check open files you see test2.txt threr" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }else{
    std::cout << "could not create/access file test2.txt"<< std::endl;
  }
}
```

The first line in the function is the most interesting here. You create a `unique_ptr` to a file with `fclose` as deleter. I think that is so nice and easy that it is probably the only correct way to do it.


