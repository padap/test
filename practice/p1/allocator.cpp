#include "allocator.h"

Allocator::Allocator(void *base, size_t size)
{   
	// printf("Allocator :: Allocator\n");
	// printf("size = %d\n",size);
	// printf("max_ptrs = %d\n",max_ptrs);
	for (int i=0;i<max_ptrs;i++){
	  array_of_pointers[i].ptr=NULL;
	  array_of_pointers[i].len=0;
	}

	  memory_status = new bool[size];
	  for (int i=0;i<size;i++)
	    memory_status[i]=0;
	  start_point=base;
	  memory_len=size;
}

Allocator::~Allocator()
{
  // printf("Allocator :: ~Allocator\n");
  delete[] memory_status;
}

// void Allocator::print_memory_status()
// {
//   int start=0;
//   int end=0;
//   int k;
//   int i=0;
//   std::cout<<"xyu";//Memory len is "<<memory_len<<std::endl;
//   while (i<memory_len){
//     if (memory_status[i]==1){
//       start=i;
//       k=i;
//       while ((memory_status[k]!=0)&&(k<memory_len))
//         k++;
//       end =k;
//       i=k+1;
//       std::cout<<"xyu";//" Is not free from "<<start<<" to "<<end<<std::endl;
//     } else {
//       i++;
//     }
// ;
//   }
// }

Pointer Allocator::alloc(size_t N)
{
  // printf("Allocator :: alloc\n");
  size_t position=-1;
  bool is_block_free=false;
  for (int i=0;i<memory_len;i++){
    if (memory_status[i]==0){
      is_block_free=true;
      for (int j=i;j<N+i;j++)
        if (memory_status[j]==1){
        is_block_free=false;
        break;
        }
      if (is_block_free==true){
        position=i;
        for (int j=i;j<N+i;j++)
          memory_status[j]=1;
        break;
      }
    }
  }
  if (position==-1){
    throw AllocError(NoMemory, "No memory");
    return Pointer();
  }
  ptr_with_len test_ptr;
  int pointer_num;
  for (int i=0;i<max_ptrs;i++){
    if (array_of_pointers[i].ptr==NULL){
      pointer_num=i;
      array_of_pointers[i].ptr=reinterpret_cast<void*>((char*)start_point+position);
      array_of_pointers[i].len=N;
      break;
    }
  }
  Pointer new_pointer=Pointer(&array_of_pointers[pointer_num]);

  return new_pointer;

}

void Allocator::free(Pointer &p){
  // printf("x");
  // printf("Allocator :: free\n");
  if (p.get()==NULL){
    std::cout<<"Invalid free"<<std::endl;
    throw AllocError(InvalidFree, "Invalid free");
    return ;
  }
  char *position_to_free=(char*)(p.get());
  size_t position=(size_t)(position_to_free-(char*)start_point);
  p.set_ptr(NULL);
  for (int i=position;i<position+p.get_size();i++)
    memory_status[i]=0;
  p.set_size(0);
}


void Allocator::realloc(Pointer &p, size_t N)
{
  // printf("Allocator :: realloc\n");
  void * newptr=p.get();
  if (newptr==NULL){

    Pointer empty_pointer=alloc(N);
    p=empty_pointer;
    return ;

  }
  char *position_to_realloc=(char*)(p.get());
  size_t position=(size_t)(position_to_realloc-(char*)start_point);
  if (p.get_size()>N) {
    for (int i=position+N;i<position+p.get_size();i++)
      memory_status[i]=0;
      p.set_size(N);
  } else{
    for (int i=position;i<position+p.get_size();i++)
      memory_status[i]=0;
    Pointer new_pointer=alloc(N);
    memmove(new_pointer.get(),p.get(),p.get_size());
    p.set_size(N);
    p.set_ptr(new_pointer.get());

  }
}

void Allocator::defrag()
{
  // printf("Allocator :: defrag\n");
  int pointer_num;
  int position;
  bool find_block_to_move=false;
  void* min_ptr=reinterpret_cast<void*>((char*)start_point+memory_len);
  while (1){
    min_ptr=reinterpret_cast<void*>((char*)start_point+memory_len);
    find_block_to_move=false;
    for (int i=0;i<max_ptrs;i++){
      if ((array_of_pointers[i].ptr!=NULL)&&(array_of_pointers[i].ptr<min_ptr)){
        position=(int)((char*)array_of_pointers[i].ptr- (char*)start_point);
        if (position>0){
          if (memory_status[position-1]==0)
          {
            min_ptr=array_of_pointers[i].ptr;
            pointer_num=i;
            find_block_to_move=true;
          }
        }
      }
    }
    if (find_block_to_move==false)
      break;
    int start_position=(int)((char*)array_of_pointers[pointer_num].ptr- (char*)start_point);
    int new_position=start_position;
    for (int i=start_position;i<start_position+array_of_pointers[pointer_num].len;i++)
      memory_status[i]=0;
    while ((memory_status[new_position]!=1) || (new_position==0))
      new_position=new_position-1;
    new_position=new_position+1;
    for (int i=new_position;i<new_position+array_of_pointers[pointer_num].len;i++)
      memory_status[i]=1;
    void *new_ptr=reinterpret_cast<void*>((char*)start_point+new_position);
    memmove(new_ptr,array_of_pointers[pointer_num].ptr,array_of_pointers[pointer_num].len);
    array_of_pointers[pointer_num].ptr=new_ptr;

  }
  return ;
}

void Pointer::set_size(size_t new_size)
{
  // printf("Pointer :: set_size\n");
  if (mptr==NULL){
    std::cout<<"Cant set size"<<std::endl;
    return;
  }
  mptr->len=new_size;
 }

 void Pointer::set_ptr(void *new_ptr)
 { 
   // printf("%d",1/0);
   // printf("Pointer :: set_ptr\n");
   if (mptr==NULL){
     std::cout<<"Cant set ptr"<<std::endl;
     return;
   }
   mptr->ptr=new_ptr;
 }
