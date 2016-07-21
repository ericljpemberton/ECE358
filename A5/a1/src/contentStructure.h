#ifndef CONTENTSTRUCTURE
#define CONTENTSTRUCTURE

#include <iostream>
#include <vector>



class ContentStructure {
private:
   class ContentNode {
   public:
      std::string content;
      int unique_id;

      ContentNode(std::string c, int u) {
         content = c;
         unique_id = u;
      }
   };


public:
   std::vector<ContentNode*> s;

   ContentStructure() {
   }

   ~ContentStructure() {
      for (int i = 0; i < s.size(); ++i) {
         delete s[i];
      }
   }

   //adds content to this structure
   void addContent(std::string c, int id) {
      ContentNode *newNode = new ContentNode(c, id);
      if (s.size() < 1) {
         s.push_back(newNode);
         return;
      }
      for (int i = s.size()-1; i >= 0; --i) {
         if (newNode->unique_id > s[i]->unique_id) {
            s.insert(s.begin()+i+1, newNode);
            break;
         }
      }

   }

   //pass in unique_id of content to be removed
   //returns 0 for successful removal
   //returns 1 if content does not exist in this structure
   int removeContent(int id) {
      if (s.size() < 1) return 1;

      int iter = 0;
      while (s[iter]->unique_id != id) {
         iter++;
         if (iter == s.size()) return 1;
      }
      delete s[iter];
      s.erase(s.begin() + iter);
      return 0;
   }

   //pass in unique_id of content you are looking for
   //returns content string if it exists here
   //returns empty string if content does not exist in this structure
   std::string lookupContent(int id) {
      if (s.size() < 1) return "";
      int iter = 0;
      while (s.at(iter)->unique_id != id) {
         iter++;
         if (iter == s.size()) return "";
      }
      return s.at(iter)->content;
   }
};

#endif
