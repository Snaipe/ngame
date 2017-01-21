#ifndef TICKABLE_H_
#define TICKABLE_H_

class tickable {
public:
    virtual void tick(double dt) = 0;
};

#endif /* !TICKABLE_H_ */
